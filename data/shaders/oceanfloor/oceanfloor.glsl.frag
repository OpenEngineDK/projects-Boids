const vec4 BACKGROUND = vec4(0.12, 0.16, 0.35, 1.0);
const float sca2 = 0.2;
const float tscale = 0.15;

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
    vec2 causticRipple = point.xz * 0.0025 + vec2(0.0, time);
    vec2 rippleEffect = sca2 * texture2D(dudvMap, causticRipple).xy;
    vec4 caustic = texture2D(causticMap, point.xz * 0.0025 + rippleEffect);

    // Calculate diffuse
    float ndotl = dot(normal, lightDir);
    float diffuseScalar = clamp(ndotl, 0.0, 1.0);
    
    // Looking up the texture values
    vec4 sand = texture2D(sandTex, gl_TexCoord[0].xy);

    vec4 diffuse = gl_LightSource[0].diffuse * diffuseScalar;
    vec4 color = sand * (gl_LightSource[0].ambient + diffuse);
    color += caustic * diffuse;

    //float fade = 0.2;//1.0 - (gl_FragCoord.z - 0.2) * 1.25;
    //float fade = gl_FragCoord.z;

    gl_FragColor = color;
    //gl_FragColor = mix(color, BACKGROUND, fade);
}
