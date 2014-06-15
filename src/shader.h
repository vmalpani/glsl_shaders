#ifndef SHADER_INC
#   define SHADER_INC

#define STRINGIFY(A) #A

/*
 Toon shading per pixel shading
 */
const char* toonVS = STRINGIFY(
varying vec3 normal;            \n
void main()                     \n
{                               \n
    normal = gl_Normal;         \n
    gl_Position = ftransform(); \n
}                               \n
);

const char* toonFS = STRINGIFY(
uniform vec3 lightDir;          \n
varying vec3 normal;            \n
void main()                     \n
{                               \n
    float intensity = dot(normalize(lightDir), normalize(normal)); \n
    if ( intensity > 0.95 )     \n
        gl_FragColor = gl_FrontMaterial.diffuse; \n
    else if ( intensity > 0.5 ) \n
        gl_FragColor = gl_FrontMaterial.diffuse*0.9; \n
    else if ( intensity > 0.25 ) \n
        gl_FragColor = gl_FrontMaterial.diffuse*0.7; \n
    else                        \n
        gl_FragColor = gl_FrontMaterial.diffuse*0.4; \n
}                               \n
);

/*
 Gouraud per vertex shading
 Reference: Lighthouse3d, Nehe, GLSL Programming wikibook
 */
const char* gouraudVS = STRINGIFY(
vec3 N;                 \n
vec3 v;                 \n
uniform int num_lights; \n
void main (void)        \n
{                                                                                   \n
    vec4 amb = vec4(0.0, 0.0, 0.0, 0.0);                                            \n
    vec4 diff = vec4(0.0, 0.0, 0.0, 0.0);                                           \n
    vec4 spec = vec4(0.0, 0.0, 0.0, 0.0);                                           \n
    vec4 globalAmb = vec4(0.0, 0.0, 0.0, 0.0);                                      \n
    float attenuation = 0.0;                                                        \n
    for(int i=0; i<num_lights; i++)                                                 \n
    {                                                                               \n
        N = normalize(gl_NormalMatrix * gl_Normal);                                 \n
        v = vec3(gl_ModelViewMatrix * gl_Vertex);                                   \n
        gl_FrontColor = vec4(0.0);                                                  \n
        
        // Transforming to eye coordinates
        vec3 D = gl_LightSource[i].position.xyz - v;                                \n
        vec3 L = normalize(D);                                                      \n
        vec3 E = normalize(-v);                                                     \n
        vec3 R = normalize(-reflect(L,N));                                          \n
        
        float vec_lec = length(D);                                                  \n
        attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
                             gl_LightSource[i].linearAttenuation * vec_lec +
                             gl_LightSource[i].quadraticAttenuation * vec_lec * vec_lec);   \n
        
        // Calculating ambient component
        amb += gl_FrontLightProduct[i].ambient;                             \n
        globalAmb = gl_LightModel.ambient * gl_FrontMaterial.ambient;       \n
        gl_FrontColor = gl_FrontLightModelProduct.sceneColor + amb + globalAmb;     \n
        
        // Calculating diffusion component
        float kd = max(dot(N,L), 0.0);                                      \n
        if (kd > 0.0){                                                      \n
            diff += gl_FrontLightProduct[i].diffuse * kd;                   \n
            diff = clamp(diff, 0.0, 1.0);                                   \n
            gl_FrontColor += diff;                                          \n
            // Calculating specular component
            float ks = pow(max(dot(R,E),0.0),0.45*gl_FrontMaterial.shininess);\n
            spec += gl_FrontLightProduct[i].specular* ks;                   \n
            spec = clamp(spec, 0.0, 1.0);                                   \n
            gl_FrontColor += spec;                                          \n
        }                                                                   \n
    }                                                                       \n
    // Incorporating emission term (improvement)
//    gl_FrontColor[3] = gl_FrontMaterial.emission[3];                        \n
    gl_FrontColor *=	attenuation;                                        \n
    gl_FrontColor.w = 1.0;                                                  \n
    gl_Position = ftransform();                                             \n
}                                                                           \n
                                  );

const char* gouraudFS = STRINGIFY(
void main()                     \n
{                               \n
    gl_FragColor = gl_Color;    \n
    gl_FragColor.w = 1.0;       \n
}                               \n
                                  );

/*
 Phong per pixel shading using reflection
 Reference: Lighthouse3d, Nehe, GLSL Programming wikibook
 */
const char* phongVS = STRINGIFY(
varying vec3 normal;       \n
varying vec3 v;            \n
void main()                \n
{                                                           \n
    normal = normalize(gl_NormalMatrix * gl_Normal);        \n
    v = vec3(gl_ModelViewMatrix * gl_Vertex);               \n
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; \n
}                                                           \n
                                );

const char* phongFS = STRINGIFY(
varying vec3 normal;                        \n
varying vec3 v;                             \n
uniform int num_lights;                     \n
void main (void)                            \n
{                                           \n
    vec4 amb = vec4(0.0, 0.0, 0.0, 0.0);    \n
    vec4 diff = vec4(0.0, 0.0, 0.0, 0.0);   \n
    vec4 spec = vec4(0.0, 0.0, 0.0, 0.0);   \n
    vec4 globalAmb = vec4(0.0, 0.0, 0.0, 0.0);                                              \n
    float attenuation = 0.0;                \n
    
    for(int i=0; i<num_lights; i++)                                                         \n
    {                                                                                       \n
        // Normalize the interpolated normal (updated)
        vec3 N = normalize(normal);                                                         \n
        
        // Transforming to eye coordinates
        vec3 D = gl_LightSource[i].position.xyz - v;                                        \n
        vec3 L = normalize(D);                                                              \n
        vec3 E = normalize(-v);                                                             \n
        vec3 R = normalize(-reflect(L,N));                                                  \n
        vec3 H = normalize(L+E);                                                            \n
        
        float vec_lec = length(D);                                                          \n
        attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
                             gl_LightSource[i].linearAttenuation * vec_lec +
                             gl_LightSource[i].quadraticAttenuation * vec_lec * vec_lec);   \n
        
        // Calculating ambient component
        amb += gl_FrontLightProduct[i].ambient;                                             \n
        globalAmb = gl_LightModel.ambient * gl_FrontMaterial.ambient;                       \n
        gl_FragColor = gl_FrontLightModelProduct.sceneColor + amb + globalAmb;              \n
        
        // Calculating diffusion component
        float kd = max(dot(N,L), 0.0);                                                      \n
        if (kd > 0.0){                                                                      \n
            diff += gl_FrontLightProduct[i].diffuse * kd;                                   \n
            diff = clamp(diff, 0.0, 1.0);                                                   \n
            gl_FragColor += diff;                                                           \n
            // Calculating specular component
            float ks = pow(max(dot(R,E),0.0),0.45*gl_FrontMaterial.shininess);              \n
            spec += gl_FrontLightProduct[i].specular* ks;                                   \n
            spec = clamp(spec, 0.0, 1.0);                                                   \n
            gl_FragColor += spec;                                                           \n
        }                                                                                   \n
    }                                                                                       \n
    // Incorporating emission term (improvement)
//    gl_FragColor[3] = gl_FrontMaterial.emission[3];                                     \n
    gl_FragColor *=	attenuation;                                                            \n
    gl_FragColor.w = 1.0;                                                                   \n
}                                                                                           \n

                                );

/*
Blinn-Phong per pixel shading using half vectors
Reference: Lighthouse3d, Nehe, GLSL Programming wikibook
*/
const char* blinnphongVS = STRINGIFY(
varying vec3 normal;            \n
varying vec3 v;                 \n
                                     
void main()                     \n
{                               \n
    normal = normalize(gl_NormalMatrix * gl_Normal);            \n
    v = vec3(gl_ModelViewMatrix * gl_Vertex);                   \n
    gl_Position = ftransform();                                 \n
}                                                               \n
                                );

const char* blinnphongFS = STRINGIFY(
varying vec3 normal;                        \n
varying vec3 v;                             \n
uniform int num_lights;                     \n
void main (void)                            \n
{                                           \n
    vec4 amb = vec4(0.0, 0.0, 0.0, 0.0);    \n
    vec4 diff = vec4(0.0, 0.0, 0.0, 0.0);   \n
    vec4 spec = vec4(0.0, 0.0, 0.0, 0.0);   \n
    vec4 globalAmb = vec4(0.0, 0.0, 0.0, 0.0);    \n
    float attenuation = 0.0;                \n
    
    for(int i=0; i<num_lights; i++)                                                         \n
    {                                                                                       \n
        // Normalize the interpolated normal (updated)
        vec3 N = normalize(normal);                                                         \n
        
        // Transforming to eye coordinates
        vec3 D = gl_LightSource[i].position.xyz - v;                                        \n
        vec3 L = normalize(D);                                                              \n
        vec3 E = normalize(-v);                                                             \n
        vec3 R = normalize(-reflect(L,N));                                                  \n
        vec3 H = normalize(L+E);                                                            \n

        float vec_lec = length(D);                                                          \n
        attenuation = 1.0 / (gl_LightSource[i].constantAttenuation +
                             gl_LightSource[i].linearAttenuation * vec_lec +
                             gl_LightSource[i].quadraticAttenuation * vec_lec * vec_lec);   \n
        
        // Calculating ambient component
        amb += gl_FrontLightProduct[i].ambient;                             \n
        globalAmb = gl_LightModel.ambient * gl_FrontMaterial.ambient;       \n
        gl_FragColor = gl_FrontLightModelProduct.sceneColor + amb + globalAmb;              \n
        
        // Calculating diffusion component
        float kd = max(dot(N,L), 0.0);                                      \n
        if (kd > 0.0){                                                      \n
            diff += gl_FrontLightProduct[i].diffuse * kd;                   \n
            diff = clamp(diff, 0.0, 1.0);                                   \n
            gl_FragColor += diff;                                           \n
            // Calculating specular component
            float ks = pow(max(dot(H,N),0.0),0.45*gl_FrontMaterial.shininess);              \n
            spec += gl_FrontLightProduct[i].specular* ks;                   \n
            spec = clamp(spec, 0.0, 1.0);                                   \n
            gl_FragColor += spec;                                           \n
            }                                                               \n
        }                                                                   \n
        // Incorporating emission term (improvement)
//        gl_FragColor[3] = gl_FrontMaterial.emission[3];                                     \n
        gl_FragColor *=	attenuation;                                                        \n
        gl_FragColor.w = 1.0;                                                               \n
    }                                                                                       \n
                                );

/*
 Checkerboard Texture using blinn-phong lighting
 */
const char* checkerVS = STRINGIFY(
varying vec3 vert_pos;                      \n
varying vec3 N;                             \n
varying vec3 v;                             \n
uniform int num_lights;                     \n
varying vec2 texCoord;                      \n
                                  
void main (void)                                                        \n
{                                                                       \n
    texCoord = gl_MultiTexCoord0.xy;                                    \n
    vec4 amb = vec4(0.0, 0.0, 0.0, 0.0);                                \n
    vec4 diff = vec4(0.0, 0.0, 0.0, 0.0);                               \n
    vec4 spec = vec4(0.0, 0.0, 0.0, 0.0);                               \n
    vec4 globalAmb = vec4(0.0, 0.0, 0.0, 0.0);                          \n
    float attenuation = 0.0;                                            \n
    for(int i=0; i<num_lights; i++)                                     \n
    {                                                                   \n
        N = normalize(gl_NormalMatrix * gl_Normal);                     \n
        v = vec3(gl_ModelViewMatrix * gl_Vertex);                       \n
        gl_FrontColor = vec4(0.0);                                      \n
        
        // Transforming to eye coordinates
        vec3 L = normalize(gl_LightSource[i].position.xyz - v);         \n
        vec3 E = normalize(-v);                                         \n
        vec3 R = normalize(-reflect(L,N));                              \n
        
        // Calculating ambient component
        amb += gl_FrontLightProduct[i].ambient;                         \n
        globalAmb = gl_LightModel.ambient * gl_FrontMaterial.ambient;   \n
        gl_FrontColor = gl_FrontLightModelProduct.sceneColor + amb + globalAmb;          \n
        // Calculating diffusion component
        float kd = max(dot(N,L), 0.0);                                  \n
        if (kd > 0.0){                                                  \n
        diff += gl_FrontLightProduct[i].diffuse * kd;                   \n
        diff = clamp(diff, 0.0, 1.0);                                   \n
        gl_FrontColor += diff;                                          \n
        // Calculating specular component
        float ks = pow(max(dot(R,E),0.0),0.45*gl_FrontMaterial.shininess);\n
        spec += gl_FrontLightProduct[i].specular* ks;                   \n
        spec = clamp(spec, 0.0, 1.0);                                   \n
        gl_FrontColor += spec;                                          \n
        }                                                               \n
    }                                                                   \n
    vert_pos = gl_Vertex.xyz;                                           \n
    gl_Position = ftransform();                                         \n
}                                                                       \n
);

const char* checkerFS = STRINGIFY(
    varying vec3 vert_pos;                              \n
    varying vec2 texCoord;                              \n
    vec2 tmpTexCoord;                                   \n
    void main()                                         \n
{                                                       \n
    tmpTexCoord = texCoord;                             \n
    gl_FragColor = gl_Color;                            \n
    
    if (tmpTexCoord.t < 0.0)                            \n
        tmpTexCoord.t = 0.0;                            \n
    else if (tmpTexCoord.t >= 4.0)                      \n
        tmpTexCoord.t = 1.0;                            \n
    else                                                \n
        tmpTexCoord.t *= 8.0;                           \n
    
    if (tmpTexCoord.s < 0.0)                            \n
        tmpTexCoord.s = 0.0;                            \n
    else if (tmpTexCoord.s >= 4.0)                      \n
        tmpTexCoord.s = 1.0;                            \n
    else                                                \n
        tmpTexCoord.s *= 8.0;                           \n

    if (mod(floor(tmpTexCoord.t)+floor (tmpTexCoord.s), 2.0) == 0.0)  \n
        gl_FragColor += vec4(0.2, 0.2, 0.2, 0.0);       \n
    else                                                \n
        gl_FragColor -= vec4(0.2, 0.2, 0.2, 0.0);       \n
    gl_FragColor.w = 1.0;                               \n
}                                                       \n
);

/*
 Normal (Bump) mapping using tangent space transformation
 using half vectors for blinn-phong lighting
 Reference: Ozone3D, Swiftless
 */
const char* bumpVS = STRINGIFY(
varying vec3 light;                         \n
varying vec3 halfv;                         \n
varying vec3 eye;                           \n
varying vec2 texCoord;                      \n
attribute vec3 tangent;                     \n
vec3 L;                                     \n
varying vec3 D;                             \n
vec3 H;                                     \n
vec3 E;                                     \n
                               
void main (void)                                        \n
{                                                       \n
    texCoord = gl_MultiTexCoord0.xy;                    \n

    // building tangent space tranformation matrix
    vec3 n = normalize (gl_NormalMatrix * gl_Normal);   \n
	vec3 t = normalize (gl_NormalMatrix * tangent);     \n
	vec3 b = cross (n, t);                              \n
    
    vec3 v = vec3(gl_ModelViewMatrix *  gl_Vertex);     \n
	D = gl_LightSource[0].position.xyz - v;             \n
	L = normalize(D);                                   \n
    E = normalize(-v);                                  \n

    // building light and view direction to tangent space
	light.x = dot (L, t);                               \n
	light.y = dot (L, b);                               \n
	light.z = dot (L, n);                               \n
    
	eye.x = dot (E, t);                                 \n
	eye.y = dot (E, b);                                 \n
	eye.z = dot (E, n);                                 \n
    
    // tranforming half vectors to tangent space
    H = normalize(E + L);                               \n
	halfv.x = dot (H, t);                               \n
	halfv.y = dot (H, b);                               \n
	halfv.z = dot (H, n);                               \n

    gl_Position = ftransform();                         \n
}                                                       \n
);

const char* bumpFS = STRINGIFY(
uniform sampler2D colorTex;                             \n
uniform sampler2D normalTex;                            \n
uniform float radiusInv;                                \n
                               
varying vec2 texCoord;                                  \n
varying vec3 light;                                     \n
varying vec3 halfv;                                     \n
varying vec3 eye;                                       \n
varying vec3 D;                                         \n

vec4 amb = vec4(0.0, 0.0, 0.0, 0.0);                    \n
vec4 diff = vec4(0.0, 0.0, 0.0, 0.0);                   \n
vec4 spec = vec4(0.0, 0.0, 0.0, 0.0);                   \n
vec4 globalAmb = vec4(0.0, 0.0, 0.0, 0.0);              \n
                               
float ks;                                               \n
float attenuation = 0.0;                                \n

void main()                                                                         \n
{                                                                                   \n
    float dist = dot(light, light);                                                 \n
    float vec_lec = length(D);                                                      \n
    attenuation = 1.0 / (gl_LightSource[0].constantAttenuation +
                         gl_LightSource[0].linearAttenuation * vec_lec +
                         gl_LightSource[0].quadraticAttenuation * vec_lec * vec_lec);   \n
   
    vec3 eyeV = normalize(eye);                                                     \n
    vec3 lightV = light * inversesqrt(dist);                                        \n
    
    // changing range of normal from (0,1) to (-1,1)
    // normal can point in any direction
    vec3 normal = normalize(2.0 * texture2D (normalTex, texCoord).rgb - 1.0);       \n
    vec4 base_img = texture2D(colorTex, texCoord);                                  \n

    float lamb_term= max (dot (lightV, normal), 0.0);                               \n
    
    // ambient light
    amb = gl_LightSource[0].ambient * gl_FrontMaterial.ambient;                     \n
    globalAmb = gl_LightModel.ambient * gl_FrontMaterial.ambient;                   \n
    gl_FragColor =	(amb+globalAmb)*base_img;                                       \n
    if (lamb_term > 0.0)                                                            \n
    {                                                                               \n
       // diffuse light
       diff = gl_LightSource[0].diffuse*gl_FrontMaterial.diffuse;                   \n
       gl_FragColor += diff * lamb_term * base_img;                                 \n

       // specular light
       spec = gl_LightSource[0].specular*gl_FrontMaterial.specular;                 \n
       ks = pow(max(dot(halfv, normal), 0.0), 2.0)  ;                               \n
       gl_FragColor += spec * ks ;                                                  \n
    }                                                                               \n
    // attenuating the final intensity
    gl_FragColor *=	attenuation;                                                    \n
    gl_FragColor.w = 1.0;                                                           \n
}                                                                                   \n
);
#endif