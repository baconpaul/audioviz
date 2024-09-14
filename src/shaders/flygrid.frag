
uniform vec2 u_resolution;
uniform float u_time;

//Sphere mapping by nimitz (twitter: @stormoid)

/*
        Little demo of different sphere uv mapping techniques.
*/

#define iTime u_time
#define iResolution u_resolution

vec3 tex(in vec2 p, in vec2 pact)
{
  float cx = p.x - 0.5;
  float cy = fract(p.y - 5. * u_time) - 0.5;
  float r = sqrt(cx * cx + cy * cy);
  return max((1.0 - 3. * r), 0.) * vec3(0.4 + 0.3 * sin(u_time * 0.7), 0.3 + pact.y * 0.7, 0.5 + 0.5 * cos(u_time * 0.4));
}

void main()
{
  vec2 op = gl_FragCoord.xy / u_resolution.xy;

  op.x -= 0.5;
  float th = 0.05 * sin( 4.* u_time) + 0.07 * cos (2.71 * u_time);
  float cth = cos(th);
  float sth = sin(th);
  mat2 rm = mat2(cth, -sth, sth, cth);
  op = op * rm;
  op.x += 0.5;

  vec3 col = vec3(0.0);
  if (op.y < 0.333) {
    vec2 np = op;
    np.y *= 3.0;
    np.y = 1.0 - np.y;


    np.x = np.x - 0.5;
    np.x *= 1.0 / (0.2 + 0.8 * np.y);
    np.y = sqrt(abs(np.y));


    col = tex(fract(np.xy * 20.), np.xy) ;
  }

  col = col + (smoothstep(0.333, 0.334, op.y) - smoothstep(0.334, 0.45, op.y)) * vec3(0.1, 0.2, 0.4);
  gl_FragColor = vec4(col, 1.0);
}