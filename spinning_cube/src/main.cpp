
#include <hGLES/hgles_context_state.h>
#include <hGLES/hgles_window.h>
#include <hGLES/hgles_input.h>
#include <glm/gtc/matrix_transform.hpp>

class OpenGLApplication :
		public hgles::WindowListener,
		public hgles::KeyboardListener
{
	hgles::Window win;
	hgles::InputSystem ins;
	hgles::ContextState gles;

	GLuint vao;
	GLuint vbo;
	GLint num_verts;
	GLuint shader_prog;
	GLint loc_model_view;
	GLint loc_projection;

	glm::mat4 view_matrix;
	float brightness;
	float rotation;

	std::vector<glm::vec3> generate_cube()
	{
		return
		{			//	position		normal
					// back
					glm::vec3(-1,-1,-1),glm::vec3(0, 0, -1),
					glm::vec3(-1, 1,-1),glm::vec3(0, 0, -1),
					glm::vec3( 1, 1,-1),glm::vec3(0, 0, -1),
					glm::vec3( 1, 1,-1),glm::vec3(0, 0, -1),
					glm::vec3( 1,-1,-1),glm::vec3(0, 0, -1),
					glm::vec3(-1,-1,-1),glm::vec3(0, 0, -1),

					// front
					glm::vec3(-1, 1, 1),glm::vec3( 0, 0,  1),
					glm::vec3(-1,-1, 1),glm::vec3( 0, 0,  1),
					glm::vec3( 1,-1, 1),glm::vec3( 0, 0,  1),
					glm::vec3( 1,-1, 1),glm::vec3( 0, 0,  1),
					glm::vec3( 1, 1, 1),glm::vec3( 0, 0,  1),
					glm::vec3(-1, 1, 1),glm::vec3( 0, 0,  1),

					// right
					glm::vec3( 1, 1, 1),glm::vec3( 1, 0,  0),
					glm::vec3( 1,-1, 1),glm::vec3( 1, 0,  0),
					glm::vec3( 1,-1,-1),glm::vec3( 1, 0,  0),
					glm::vec3( 1,-1,-1),glm::vec3( 1, 0,  0),
					glm::vec3( 1, 1,-1),glm::vec3( 1, 0,  0),
					glm::vec3( 1, 1, 1),glm::vec3( 1, 0,  0),

					//left
					glm::vec3(-1, 1,-1),glm::vec3(-1, 0, 0),
					glm::vec3(-1,-1, 1),glm::vec3(-1, 0, 0),
					glm::vec3(-1, 1, 1),glm::vec3(-1, 0, 0),
					glm::vec3(-1, 1,-1),glm::vec3(-1, 0, 0),
					glm::vec3(-1,-1,-1),glm::vec3(-1, 0, 0),
					glm::vec3(-1,-1, 1),glm::vec3(-1, 0, 0),

					// top
					glm::vec3(-1, 1,-1),glm::vec3( 0, 1,  0),
					glm::vec3(-1, 1, 1),glm::vec3( 0, 1,  0),
					glm::vec3( 1, 1, 1),glm::vec3( 0, 1,  0),
					glm::vec3( 1, 1, 1),glm::vec3( 0, 1,  0),
					glm::vec3( 1, 1,-1),glm::vec3( 0, 1,  0),
					glm::vec3(-1, 1,-1),glm::vec3( 0, 1,  0),

					// bottom
					glm::vec3(-1,-1,-1),glm::vec3(0, -1, 0),
					glm::vec3( 1,-1,-1),glm::vec3(0, -1, 0),
					glm::vec3(-1,-1, 1),glm::vec3(0, -1, 0),
					glm::vec3(-1,-1, 1),glm::vec3(0, -1, 0),
					glm::vec3( 1,-1,-1),glm::vec3(0, -1, 0),
					glm::vec3( 1,-1, 1),glm::vec3(0, -1, 0)
		};


	}
public:

	OpenGLApplication():win(800,600)
	{
		// setup the system:
		// init the input system. It needs the window to do so.
		ins.init(&win);


		// in order to access events we need to register as listener
		win.add_window_listener(this);
		ins.add_keyboard_listener(this);

		// lets create some simple geometry
		auto vd =  generate_cube();
		num_verts = static_cast<GLint>(vd.size()/2);
		gles.GenVertexArrays(1,&vao);
		gles.GenBuffers(1,&vbo);
		gles.BindVertexArray(vao);
		gles.BindBuffer(GL_ARRAY_BUFFER,vbo);
		gles.BufferData(GL_ARRAY_BUFFER,
						static_cast<GLsizeiptr>(sizeof(glm::vec3)*vd.size()),
						vd.data(),
						GL_STATIC_DRAW);

		// choose binding and attribute id ...
		const GLuint pos_attib_id = 0;
		const GLuint nrm_attib_id = 1;

		// set the attributes format: how does one of this attributes look like
		// insida a set of attributes.
		gles.EnableVertexAttribArray(pos_attib_id);
		gles.VertexAttribPointer(pos_attib_id,
								 3,
								 GL_FLOAT,
								 GL_FALSE,
								 6*sizeof(float),
								 0);

		gles.EnableVertexAttribArray(nrm_attib_id);
		gles.VertexAttribPointer(nrm_attib_id,
								 3,
								 GL_FLOAT,
								 GL_FALSE,
								 6*sizeof(float),
								 reinterpret_cast<void*>(3*sizeof(float)));

		// create a shader using a util_ function ... making it easy to
		// create a shader from a strings.

		shader_prog = gles.CreateProgam();
		auto vs = gles.util_CreateShader(GL_VERTEX_SHADER,
										 "#version 100\n "
										 "precision highp float;\n"
										 "attribute vec3 pos; \n"
										 "attribute vec3 nrm; \n"
										 "varying vec3 v_nrm; \n"
										 "uniform mat4 mv; \n"
										 "uniform mat4 proj; \n"
										 "void main(){\n"
										 "gl_Position = proj*mv*vec4(pos,1.0);\n"
										 "v_nrm = nrm;\n"
										 "}");
		auto fs = gles.util_CreateShader(GL_FRAGMENT_SHADER,
										 "#version 100\n"
										 "precision highp float;\n"
										 "varying vec3 v_nrm; \n"
										 "void main(){\n"
										 "vec3 clr = (normalize(v_nrm)+vec3(1))*0.5;\n"
										 "gl_FragColor = vec4(clr,1.0);\n}");
		gles.AttachShader(shader_prog,vs);
		gles.AttachShader(shader_prog,fs);
		gles.BindAttribLocation(shader_prog,pos_attib_id,"pos");
		gles.BindAttribLocation(shader_prog,nrm_attib_id,"nrm");

		gles.LinkProgram(shader_prog);

		gles.DeleteShader(vs);
		gles.DeleteShader(fs);
		// we wont use any other program so we can bind it and keep it that way!
		gles.UseProgram(shader_prog);

		// store uniform locations
		loc_model_view = gles.GetUniformLocation(shader_prog,"mv");
		loc_projection = gles.GetUniformLocation(shader_prog,"proj");

		// set some inital uniform values

		gles.Uniform(loc_projection,
					 glm::perspective(glm::radians(60.f),4.0f/3.0f,0.1f,100.0f));
		view_matrix = glm::lookAt(glm::vec3(5,2,5),
								  glm::vec3(0,0,0),
								  glm::vec3(0,1,0));
		gles.Uniform(loc_model_view,view_matrix);

		// set up some variables for animation
		brightness = 1.0f;
		rotation = 0.0f;
		gles.Enable(GL_CULL_FACE);
		gles.Enable(GL_DEPTH_TEST);
	}

	void start_rendering()
	{

		while(!win.should_close())
		{
			gles.ClearColor(brightness,brightness,brightness,1);


			if(ins.is_key_down(hgles::K_UP) && brightness < 1.0f)
				brightness+=0.01f;
			if(ins.is_key_down(hgles::K_DOWN) && brightness >=0.0f)
				brightness-=0.01f;
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


			rotation+=1.0f;
			rotation = rotation>360?rotation-360:rotation;

			gles.Uniform(loc_model_view,glm::rotate(view_matrix,
													glm::radians(rotation),
													glm::vec3(0,1,0)));

			gles.Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			gles.BindVertexArray(vao);
			gles.DrawArrays(GL_TRIANGLES,0,num_verts);

			// swap buffers
			win.swap_buffers();
			// check for new events!
			win.poll_events();
			ins.poll_events();
		}
	}

	void size_changed(const int w, const int h)
	{
		glViewport(0,0,w,h);
		const float ar =  static_cast<float>(w)/static_cast<float>(h);
		gles.Uniform(loc_projection, glm::perspective(glm::radians(60.f),
													  ar,
													  0.1f,100.0f));
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

	OpenGLApplication app;
	app.start_rendering();

	return 0;
}
