
#include <hGLES/hgles_context_state.h>
#include <hGLES/hgles_window.h>
#include <hGLES/hgles_input.h>

#include <glm/glm/gtc/matrix_transform.hpp>
#include <td/td.h>
class OpenGLApplication :
		public hgles::WindowListener,
		public hgles::KeyboardListener
{
	hgles::Window win;
	hgles::InputSystem ins;
	hgles::ContextState gles;

	GLuint vao;
	GLuint vbo;

	GLuint shader_prog;
	GLint loc_sampler;
	GLuint texture;

	float img_ar;
public:

	OpenGLApplication(const std::string& td_p):win(800,600)
	{
		td::TextureData td;
		td.read(td_p);
		// setup the system:
		// init the input system. It needs the window to do so.
		ins.init(&win);


		// in order to access events we need to register as listener
		win.add_window_listener(this);
		ins.add_keyboard_listener(this);

		// lets create some simple geometry
		std::vector<glm::vec2> vd={
			glm::vec2(0,0),glm::vec2(1,0), glm::vec2(1,1),
			glm::vec2(1,1),glm::vec2(0,1), glm::vec2(0,0)};

		gles.GenVertexArrays(1,&vao);
		gles.GenBuffers(1,&vbo);
		gles.BindVertexArray(vao);
		gles.BindBuffer(GL_ARRAY_BUFFER,vbo);
		gles.BufferData(GL_ARRAY_BUFFER,
						static_cast<GLsizeiptr>(sizeof(glm::vec2)*vd.size()),
						vd.data(),
						GL_STATIC_DRAW);

		// choose binding and attribute id ...
		const GLuint pos_attib_id = 0;

		// set the attributes format: how does one of this attributes look like
		// insida a set of attributes.
		gles.EnableVertexAttribArray(pos_attib_id);
		gles.VertexAttribPointer(pos_attib_id,
								 2,
								 GL_FLOAT,
								 GL_FALSE,
								 2*sizeof(float),
								 0);

		// create a shader using a util_ function ... making it easy to
		// create a shader from a strings.

		shader_prog = gles.CreateProgam();
		auto vs = gles.util_CreateShader(GL_VERTEX_SHADER,
										 "#version 100\n "
										 "precision highp float;\n"
										 "attribute vec2 pos; \n"
										 "varying vec2 tex_coord;\n"
										 "void main(){\n"
										 "gl_Position = vec4((pos*2.0-vec2(1)),0.0,1.0);\n"
										 "tex_coord = pos;\n"
										 "tex_coord.y = 1.0 -tex_coord.y;\n"
										 "}");
		auto fs = gles.util_CreateShader(GL_FRAGMENT_SHADER,
										 "#version 100\n"
										 "precision highp float;\n"
										 "varying vec2 tex_coord;\n"
										 "uniform sampler2D tex;\n"
										 "void main(){\n"
										 "vec3 clr = texture2D(tex,tex_coord).rgb;\n"
										 "gl_FragColor = vec4(clr,1.0);\n}");
		gles.AttachShader(shader_prog,vs);
		gles.AttachShader(shader_prog,fs);
		gles.BindAttribLocation(shader_prog,pos_attib_id,"pos");

		gles.LinkProgram(shader_prog);

		gles.DeleteShader(vs);
		gles.DeleteShader(fs);
		// we wont use any other program so we can bind it and keep it that way!
		gles.UseProgram(shader_prog);

		// store uniform locations
		loc_sampler = gles.GetUniformLocation(shader_prog,"tex");
		gles.Uniform(loc_sampler,0);
		gles.ActiveTexture(GL_TEXTURE0);


		gles.GenTextures(1,&texture);
		gles.BindTexture(GL_TEXTURE_2D,texture);
		for(const auto& l : td)
		{
			gles.TexImage2D(GL_TEXTURE_2D,l.lvl,GL_RGBA,l.w,l.h,0,(GLenum)l.frmt,(GLenum)l.type,l.data);
		}
		gles.TexParametr(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		gles.TexParametr(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);
		gles.TexParametr(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
		gles.TexParametr(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

		img_ar = static_cast<float>(td.begin()->w)/td.begin()->h;
		win.set_size(td.begin()->w,td.begin()->h);
	}

	void start_rendering()
	{

		while(!win.should_close())
		{
			gles.ClearColor(0,0,0,1);

			if(ins.is_key_down(hgles::K_KP_ADD))
				win.set_size(win.get_size()+glm::ivec2(10));
			if(ins.is_key_down(hgles::K_KP_SUBTRACT))
				win.set_size(win.get_size()-glm::ivec2(10));

			if(ins.is_key_down(hgles::K_W))
				win.set_position(win.get_position()+glm::ivec2(0,-10));
			if(ins.is_key_down(hgles::K_A))
				win.set_position(win.get_position()+glm::ivec2(-10,0));
			if(ins.is_key_down(hgles::K_S))
				win.set_position(win.get_position()+glm::ivec2(0,10));
			if(ins.is_key_down(hgles::K_D))
				win.set_position(win.get_position()+glm::ivec2(10,0));




			gles.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			gles.BindVertexArray(vao);
			gles.DrawArrays(GL_TRIANGLES,0,6);

			// swap buffers
			win.swap_buffers();
			// check for new events!
			win.poll_events();
			ins.poll_events();
		}
	}

	void size_changed(const int w, const int h)
	{

		const float ar =  static_cast<float>(w)/static_cast<float>(h);
		if(img_ar < ar)
		{
			int target_w = img_ar*h;
			gles.Viewport((w-target_w)*0.5f,0,target_w,h);
		}
		else
		{
			int target_h = w/img_ar;
			gles.Viewport(0,(h-target_h)*0.5f,w,target_h);
		}

	}

	void key_down(const hgles::Key k)
	{
		if(k == hgles::K_ESCAPE || k == hgles::K_Q)
		{
			win.set_should_close();
		}
		if(k == hgles::K_F)
			win.toggle_fullscreen();
		if(k == hgles::K_L)
			win.toggle_decoration();
	}

};
int main()
{
	OpenGLApplication app("monarch.td");
	app.start_rendering();

	return 0;
}
