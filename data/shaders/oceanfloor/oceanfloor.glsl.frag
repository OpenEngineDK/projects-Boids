const vec4 BACKGROUND = vec4(0.12, 0.16, 0.35, 1.0);
const float BUMP_FACTOR = 4.0;

uniform float time;

uniform sampler2D sandTex;
uniform sampler2D sandBump;
uniform sampler2D causticMap;
uniform sampler2D normalMap;

uniform vec3 lightDir; // Should be pre-normalized. Or else the world will BURN IN RIGHTEOUS FIRE!!

varying vec3 eyeDirection;
varying vec3 point;

void main()
{
    vec3 eyeDir = normalize(eyeDirection);
    
    // Extract normal and calculate tangent and binormal
    vec3 normal = normalize(texture2D(normalMap, gl_TexCoord[1].xy).xyz);

    // Calculate tangent and binormal. Taking advantage of what we
    // know of the heightmap
    vec3 tangent = normalize(vec3(-normal.y * 0.5, normal.x, 0.0));
    vec3 binormal = normalize(vec3(0.0, normal.z, -normal.y * 0.5));

    // Extract the bump and rotate the bump into normalspace
    vec3 bump = texture2D(sandBump, gl_TexCoord[0].xy).xzy * 2.0 - 1.0;
    bump.y *= BUMP_FACTOR;
    bump = vec3(dot(bump, tangent), dot(bump, normal), dot(bump, binormal));
    bump = normalize(bump);

    // Calculate caustics
    vec2 causticRipple = vec2(0.0, time) * 2.0;
    vec4 caustic = texture2D(causticMap, point.xz * 0.0025 + causticRipple);
    caustic += texture2D(causticMap, point.xz * 0.0025 - causticRipple);

    // Calculate diffuse
    float ndotl = dot(bump, lightDir);
    float diffuseScalar = clamp(ndotl, 0.0, 1.0);
    
    // Looking up the texture values
    vec4 sand = texture2D(sandTex, gl_TexCoord[0].xy);

    vec4 diffuse = gl_LightSource[0].diffuse * diffuseScalar;
    vec4 color = sand * (gl_LightSource[0].ambient + diffuse);
    color += caustic * diffuse;

    // TODO far clipping plane should be a uniform
    float farClip = 4000.0;
    float fade = clamp(gl_FragCoord.z / gl_FragCoord.w / farClip, 0.0, 1.0);

    gl_FragColor = mix(color, BACKGROUND, fade);
}
