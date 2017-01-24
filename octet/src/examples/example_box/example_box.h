﻿////////////////////////////////////////////////////////////////////////////////
//
// (C) Andy Thomason 2012-2014
//
// Modular Framework for OpenGLES2 rendering on multiple platforms.
//
#include <stdlib.h>


namespace octet {
  /// Scene containing a box with octet.
  class example_box : public app {
    // scene for drawing box
    ref<visual_scene> app_scene;

    std::vector<std::tuple<vec3, vec3>> input;
    std::vector<float> vertBuff;
    GLuint vertex_buffer;
    shader road_shader;

    std::string load_file(const char* file_name) {
      std::ifstream is(file_name);
      if (is.bad() || !is.is_open()) return nullptr;
      char buffer[2048];
      // loop over lines
      std::string out;
      while (!is.eof()) {
        is.getline(buffer, sizeof(buffer));
        out += buffer;
        out += "\n";
      }
      //printf("%s", out.c_str());
      return out;
    }

  public:
    /// this is called when we construct the class before everything is initialised.
    example_box(int argc, char **argv) : app(argc, argv) {
    }

    /// this is called once OpenGL is initialized
    void app_init() {
      app_scene =  new visual_scene();
      app_scene->create_default_camera_and_lights();

      glGenBuffers(1, &vertex_buffer); // Sets up our vertex array buffer for rendering
      road_shader.init(load_file("shaders/tree.vert").c_str(), load_file("shaders/tree.frag").c_str()); // loads, compiles and links our shader programs

      float TRACK_WIDTH = 0.5f;
      float DETAIL_STEP = 0.00001f;

      //input = std::vector<std::tuple<vec3, vec3>>(5);
      //input[0] = std::tuple<vec3, vec3>(vec3(-0.75f, -0.5f, 0), vec3(-1, 0, 0));
      //input[1] = std::tuple<vec3, vec3>(vec3(-0.5f, 0, 0), vec3(-1, 1, 0));
      //input[2] = std::tuple<vec3, vec3>(vec3(0, 0.5, 0), vec3(0, 1, 0));
      //input[3] = std::tuple<vec3, vec3>(vec3(0.5f, 0, 0), vec3(1, 1, 0));
      //input[4] = std::tuple<vec3, vec3>(vec3(0.75f, -0.5f, 0), vec3(1, 0, 0));

      vertBuff = std::vector<float>();

      for (float t = 0.0f; t <= 1.0f; t += DETAIL_STEP) {
        vec3 pos = get_bezier_point(t);
        vec3 tan = get_bezier_tangent(t);
        vec3 norm = tan.cross(vec3(0, 0, 1)); // Get normal from tangent.
        /*vec3 pos = std::get<0>(point_norm);
        vec3 norm = std::get<1>(point_norm);*/
        norm = norm.normalize() * TRACK_WIDTH * 0.5f; // Create track radius
        vec3 p1 = pos - norm; // Calculate border vertex locations
        vec3 p2 = pos + norm;
        vertBuff.push_back(p1[0]); // Add vertex data (3 Floats (x, y and y)) to the buffer
        vertBuff.push_back(p1[1]);
        vertBuff.push_back(p1[2]);
        vertBuff.push_back(p2[0]);
        vertBuff.push_back(p2[1]);
        vertBuff.push_back(p2[2]);
      }

      /*for (float &f : vertBuff) {
        printf("%f \n", f);
      }*/



      /*material *red = new material(vec4(1, 0, 0, 1));
      mesh *box = new mesh_box(vec3(4));
      scene_node *node = new scene_node();
      app_scene->add_child(node);
      app_scene->add_mesh_instance(new mesh_instance(node, box, red));*/
    }

    vec3 get_bezier_point(float t) {
      vec3 way1(-0.75f, -0.5f, 0);
      vec3 way2(0, 1.0f, 0);
      vec3 way3(0.75f, -0.5f, 0);

      vec3 point(0,0,0);
      point[0] = (1 - t) * (1 - t) * way1[0] + 2 * (1 - t) * t * way2[0] + t * t * way3[0];
      point[1] = (1 - t) * (1 - t) * way1[1] + 2 * (1 - t) * t * way2[1] + t * t * way3[1];
      return point;
    }
    vec3 get_bezier_tangent(float t) {
      //P(1)1 = (1 − t)P0 + tP1   (= P0 + t(P1 − P0))
      //P(1)2 = (1 − t)P1 + tP2   (= P1 + t(P2 - P1))
      vec3 way1(-0.75f, -0.5f, 0);
      vec3 way2(0, 1.0f, 0);
      vec3 way3(0.75f, -0.5f, 0);

      vec3 P11 = way1 + t * (way2 - way1);
      vec3 P12 = way2 + t * (way3 - way2);

      vec3 tan = P12 - P11;
      return tan;
    }

    /// this is called to draw the world
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      

      // update matrices. assume 30 fps.
      app_scene->update(1.0f/30);

      // draw the scene
      app_scene->render((float)vx / vy);

      glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
      glBufferData(GL_ARRAY_BUFFER, vertBuff.size() * sizeof(GLfloat), &vertBuff[0], GL_STATIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
      glEnableVertexAttribArray(attribute_pos);

      glUseProgram(road_shader.get_program());
      glDrawArrays(GL_TRIANGLE_STRIP, 0, vertBuff.size() / 3);
      glBindVertexArray(attribute_pos);
    }
  };
}
