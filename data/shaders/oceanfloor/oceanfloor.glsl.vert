const float tscale = 0.25;

uniform vec3 viewPos;
uniform float baseDistance;
uniform float invIncDistance;

uniform float time;
uniform vec3 lightDir;

varying vec3 eyeDirection;
varying vec2 causticRipple; //moving texcoords
varying vec3 point;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_TexCoord[1] = gl_MultiTexCoord1;

    vec4 vertex = vec4(gl_Vertex.xyz, 1.0);
    float morphValue = gl_Vertex.w;
    
    vec3 patchCenter = vec3(gl_Normal.x, 0, gl_Normal.y);
    float lod = gl_Normal.z;
    float dist = distance(viewPos, patchCenter) - baseDistance;

    float morphScale = clamp(dist * invIncDistance - lod, 0.0, 1.0);
    vertex.y += morphScale * morphValue;

    // Calculate the eyeDir relative to the vertex.
    eyeDirection = viewPos - vertex.xyz;

    point = vertex.xyz;

    // Project the vertex along the lightDir onto the xz plane, given
    // by y = 0, and use this coord for caustics.
    //float s = -vertex.y / lightDir.y;
    //causticRipple = vec2(vertex.x + s * lightDir.x, vertex.z + s * lightDir.z);

    // texcoords for making the water ripple
    causticRipple = (gl_MultiTexCoord0.xy + vec2(0.0, time)) * tscale;

    // Doing the stuff
    gl_ClipVertex = gl_ModelViewMatrix * vertex;
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
