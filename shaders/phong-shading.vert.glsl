#version 130


in vec4 v_coord;
in vec3 v_normal;
in vec2 v_text_coord;

out vec2 f_text_coord;
out vec4 position;  // position of the vertex (and fragment) in world space
out vec3 varyingNormalDirection;  // surface normal vector in world space
uniform mat4 m, v, p;
uniform mat3 m_3x3_inv_transp;

void main()
{
  f_text_coord = v_text_coord;
  position = m * v_coord;
  varyingNormalDirection = normalize(m_3x3_inv_transp * v_normal);
  mat4 mvp = p*v*m;
  gl_Position = mvp * v_coord;
}
