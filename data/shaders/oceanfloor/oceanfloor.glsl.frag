const vec4 BACKGROUND = vec4(0.12, 0.16, 0.35, 1.0);
const float sca2 = 0.2;
const float tscale = 0.25;

uniform float time;

uniform sampler2D sandTex;
uniform sampler2D causticMap;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;

uniform vec3 lightDir; // Should be pre-normalized. Or else the world will BURN IN RIGHTEOUS FIRE!!

varying vec3 eyeDirection;
varying vec2 causticRipple; //moving texcoords
varying vec3 point;

void main()
{
    vec3 eyeDir = normalize(eyeDirection);
    
    // Extract normal
    vec3 normal = normalize(texture2D(normalMap, gl_TexCoord[1].xy).xyz);

    // Extract ocean normal
    vec2 causticRipple = (point.xz * 0.01 + vec2(0.0, time)) * tscale;
    vec2 rippleEffect = sca2 * texture2D(dudvMap, causticRipple).xy;
    vec4 caustic = texture2D(causticMap, point.xz * 0.003 + rippleEffect);

    // Calculate diffuse
    float ndotl = dot(normal, lightDir);
    float diffuse = clamp(ndotl, 0.0, 1.0);
    
    // Looking up the texture values
    vec4 sand = texture2D(sandTex, gl_TexCoord[0].xy);
    
    vec4 text = caustic + sand;
    
    vec4 color = text * (gl_LightSource[0].ambient + gl_LightSource[0].diffuse * diffuse);

    gl_FragColor = color;
}
