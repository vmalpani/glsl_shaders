
//
//  phong_shader.h
//  PA3
//
//  Created by Vaibhav Malpani on 05/04/14.
//
//

#ifndef PA3_phong_shader_h
#define PA3_phong_shader_h

#define STRINGIFY(A) #A

//const char* phongVS = STRINGIFY(
//                               varying vec3 N;            \n
//                               varying vec3 v;            \n
//                               void main()                     \n
//{                               \n
//    N = normalize(gl_NormalMatrix * gl_Normal);         \n
//    v = vec3(gl_ModelViewMatrix * gl_Vertex);         \n
//    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; \n
//}                               \n
//                               );
//
//const char* phongFS = STRINGIFY(
//                                varying vec3 N;     \n
//                                varying vec3 v;     \n
//                                void main (void)    \n
//{                               \n
//    vec3 L = normalize(gl_LightSource[0].position.xyz - v); \n
//    vec3 E = normalize(-v); \n
//    vec3 R = normalize(-reflect(L,N));  \n
//    vec4 Iamb = gl_FrontLightProduct[0].ambient; \n
//    vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(N,L), 0.0); \n
//    Idiff = clamp(Idiff, 0.0, 1.0); \n
//    
//    vec4 Ispec = gl_FrontLightProduct[0].specular* pow(max(dot(R,E),0.0),0.3*gl_FrontMaterial.shininess); \n
//    Ispec = clamp(Ispec, 0.0, 1.0); \n
//    gl_FragColor = gl_FrontLightModelProduct.sceneColor + Iamb + Idiff + Ispec;     \n
//}                               \n
//                               );

#endif
