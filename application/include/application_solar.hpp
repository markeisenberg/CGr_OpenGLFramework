#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  struct orb{
    
    //Declaring the properties
      bool lunar;
      bool stars;
    float scaling;
    float rotation;
    float distance_to_origin;

    //assigning nexessary value types
    orb (float s, float r, float d, bool m = false, bool n = false):
    scaling{s},
    rotation{r},
    distance_to_origin{d},
    lunar{m} {};
      
  };
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // update uniform locations and values
  void uploadUniforms();
  // update projection matrix
  void updateProjection();
  // react to key input
  void keyCallback(int key, int scancode, int action, int mods);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);

  // draw all objects
  void render() const;

 protected:
  void initializeShaderPrograms();
  void initializeGeometry();
  void updateView();

  void upload_planet_transforms(orb  &p) const;

  // cpu representation of model
  model_object planet_object, star_object;
};

#endif
