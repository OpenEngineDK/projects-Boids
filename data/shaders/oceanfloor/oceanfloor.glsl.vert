uniform vec3 viewPos;
uniform float baseDistance;
uniform float invIncDistance;

uniform vec3 lightDir;

varying vec3 eyeDirection;
varying vec3 point;

void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;

    vec4 vertex = vec4(gl_Vertex.xyz, 1.0);
    float morphValue = gl_Vertex.w;
    
    vec3 patchCenter = vec3(gl_Normal.x, 0, gl_Normal.y);
    float lod = gl_Normal.z;
    float dist = distance(viewPos, patchCenter) - baseDistance;

    float morphScale = clamp(dist * invIncDistance - lod, 0.0, 1.0);
    vertex.y += morphScale * morphValue;

    point = vertex.xyz;

    // Calculate the eyeDir relative to the vertex.
    eyeDirection = viewPos - point;

    // Doing the stuff
    gl_ClipVertex = gl_ModelViewMatrix * vertex;
	gl_Position = gl_ModelViewProjectionMatrix * vertex;
}
