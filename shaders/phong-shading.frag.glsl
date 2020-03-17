#version 130

// From vertex shader
in vec2 f_text_coord;
in vec4 position;  // position of the vertex (and fragment) in world space
in vec3 varyingNormalDirection;  // surface normal vector in world space

// The end result of this shader
out vec4 color;

uniform mat4 m, v, p;
uniform mat4 v_inv;
uniform int numberOfLights;

// declaration of a Material structure
struct Material
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    float shininess;
    bool has_diffuse_map;
    sampler2D diffuse_map;
};

// Definition of a light source structure
struct LightSource
{
  bool enabled;
  vec4 position;
  vec4 diffuse;
  vec4 specular;
};

const int MaxNumberOfLights = 10;

// This is the uniforms that our program communicates with
uniform LightSource lights[MaxNumberOfLights];

// Some hard coded default ambient lighting
vec4 scene_ambient = vec4(0.2, 0.2, 0.2, 1.0);

// The front surface material
uniform Material material;

void main()
{
  vec3 normalDirection = normalize(varyingNormalDirection);
  vec3 viewDirection = normalize(vec3(v_inv * vec4(0.0, 0.0, 0.0, 1.0) - position));
  vec3 lightDirection;
  float attenuation;

  // initialize total lighting with ambient lighting
  vec3 totalLighting = vec3(scene_ambient) * vec3(material.ambient);

  // for all light sources
  for (int index = 0; index < numberOfLights; index++) {
    LightSource light = lights[index];
    if (!light.enabled) {
      continue;
    }
    if (0.0 == light.position.w) // directional light?
    {
      attenuation = 1.0; // no attenuation
      lightDirection = normalize(vec3(light.position));
    }
    else // point light or spotlight (or other kind of light) 
    {
      vec3 positionToLightSource = vec3(light.position - position);
      float distance = length(positionToLightSource);
      lightDirection = normalize(positionToLightSource);
      attenuation = 1.0;
    }

    vec3 diffuseReflection = attenuation
      * vec3(light.diffuse) * vec3(material.diffuse)
      * max(0.0, dot(normalDirection, lightDirection));

    vec3 specularReflection;
    if (dot(normalDirection, lightDirection) < 0.0) // light source on the wrong side?
    {
      specularReflection = vec3(0.0, 0.0, 0.0); // no specular reflection
    }
    else // light source on the right side
    {
      specularReflection = attenuation * vec3(light.specular) * vec3(material.specular)
        * pow(max(0.0, dot(reflect(-lightDirection, normalDirection), viewDirection)), material.shininess);
    }

    totalLighting = totalLighting + diffuseReflection + specularReflection;
  }


  color = vec4(totalLighting, 1.0);
  
  // How we could check for a diffuse texture map
  if (material.has_diffuse_map) {
    color = texture2D(material.diffuse_map, f_text_coord) * color;
  }
}
