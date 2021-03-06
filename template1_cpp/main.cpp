#include "common.h"
// #include "Image.h"
#include "Player.h"
#include "unistd.h"

#define GLFW_DLL
#include <GLFW/glfw3.h>

constexpr GLsizei WINDOW_WIDTH = block_size * roomSize, WINDOW_HEIGHT = block_size * roomSize;

struct InputState
{
  bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
  GLfloat lastX = 400, lastY = 300; //исходное положение мыши
  bool firstMouse = true;
  bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
  bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
GLfloat currentFrame = 0.0f;


void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
	default:
		if (action == GLFW_PRESS)
      Input.keys[key] = true;
		else if (action == GLFW_RELEASE)
      Input.keys[key] = false;
	}
}

STATE processPlayerMovement(Player &player, Image &screen)
{
  if (Input.keys[GLFW_KEY_W])
    return player.ProcessInput(MovementDir::UP, screen, currentFrame);
  else if (Input.keys[GLFW_KEY_S])
    return player.ProcessInput(MovementDir::DOWN, screen, currentFrame);
  if (Input.keys[GLFW_KEY_A])
    return player.ProcessInput(MovementDir::LEFT, screen, currentFrame);
  else if (Input.keys[GLFW_KEY_D])
    return player.ProcessInput(MovementDir::RIGHT, screen, currentFrame);

  if (player.dying && (currentFrame - player.prev_lost_life_time) > 1) {
    player.lost_life = true;
    player.lives--;
    player.prev_lost_life_time = currentFrame;
    if (player.lives) {
      return STATE::PLAYING;
    } else {
      return STATE::LOSE;
    }
  } else {
    player.lost_life = false;
  }
  return STATE::PLAYING;
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    Input.captureMouse = !Input.captureMouse;

  if (Input.captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input.capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (Input.firstMouse)
  {
    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
    Input.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - Input.lastX;
  GLfloat yoffset = Input.lastY - float(ypos);

  Input.lastX = float(xpos);
  Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}


int initGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
  std::cout << "W, A, S, D - movement  "<< std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}

void render(Image &screen, GLFWwindow*  window) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
  glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screen.Data()); GL_CHECK_ERRORS;

	glfwSwapBuffers(window);
  return;
}

int main(int argc, char** argv)
{
	if(!glfwInit())
    return -1;

	// glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	// glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  Image screenBuffer("../Rooms/rooms_map.txt");
  std::cout << (screenBuffer.Data() == nullptr) << std::endl;
  std::cout << WINDOW_WIDTH << " " << WINDOW_HEIGHT << std::endl;

  GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "task1 base project", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 

	glfwSetKeyCallback        (window, OnKeyboardPressed); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback     (window, OnMouseScroll);

	if(initGL() != 0) 
		return -1;
	
  //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

  Point starting_pos{.x = WINDOW_WIDTH / 2 - tileSize / 2, .y = WINDOW_HEIGHT / 2 - tileSize / 2};
	Player player{starting_pos};

  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;

  std::cout << WINDOW_WIDTH << " " << WINDOW_HEIGHT << std::endl;
  //game loop

  STATE game_state(STATE::PLAYING);
  bool run_loop = true;
  int count = 0;

	while (!glfwWindowShouldClose(window) && run_loop) {
    currentFrame = glfwGetTime();
		deltaTime += currentFrame - lastFrame;
		lastFrame = currentFrame;

    if (deltaTime < 0.003) {
      render(screenBuffer, window);
    } else {
      switch(game_state) {
        case STATE::WIN:
          screenBuffer.winData();
          std::cout << "YOU WIN" << std::endl;
          render(screenBuffer, window);
          sleep(2);
          run_loop = false;
          break;
        case STATE::LOSE:
          screenBuffer.loseData();
          std::cout << "DIED! YOU LOSE" << std::endl;
          render(screenBuffer, window);
          sleep(2);
          run_loop = false;
          break;
        case STATE::PLAYING:
          glfwPollEvents();

          game_state = processPlayerMovement(player, screenBuffer);
          if (game_state == STATE::ROOM_CHANGE) {
            float alpha = 0.01;
            for (int i = 0; i <= 100; ++i) {
              screenBuffer.blend_prev_room(alpha * i);
              render(screenBuffer, window);
            }
            for (int i = 0; i <= 100; ++i) {
              screenBuffer.blend_cur_room(alpha * i);
              render(screenBuffer, window);
            }
            for(int i = 0; i < player.num_lives(); ++i) {
              for(int y = 0; y < block_size; ++y) {
                for(int x = 0; x < block_size; ++x) {
                  screenBuffer.PutPixelLife(x, y, i);
                }
              }
            }
            game_state = STATE::PLAYING;
          }

          player.Draw(screenBuffer);
          render(screenBuffer, window);
          break;
        default:
          break;
      }
      deltaTime = 0.0;
    }
    count++;
	}

	glfwTerminate();
	return 0;
}
