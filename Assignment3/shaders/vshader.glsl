#version 130

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;
uniform mat4 ModelView;
uniform mat4 ModelView2;
uniform mat4 Projection;
uniform int DRAWING_MODE_GLOBAL;
uniform int Secondary_Mode;
uniform int L1_Toggle;
uniform int L2_Toggle;
uniform vec4 Light1Position;
uniform vec4 Light2Position;

in vec4 vPosition;
in vec3 vNormal;
in vec2 vCoords;

out vec4 color;
out vec2 v_coords;
out  vec3 fN;
out  vec3 fE;
out  vec3 Light1;
out  vec3 Light2;


void main() {

    if (DRAWING_MODE_GLOBAL == 0) {
        color = vec4(0,0,0,1);
    }

    if (DRAWING_MODE_GLOBAL == 1 || (Secondary_Mode == 1 && DRAWING_MODE_GLOBAL == 4)) {
        vec4 light1_color = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 light2_color = vec4(0.0, 0.0, 0.0, 1.0);

        vec3 pos = (ModelView2 * vPosition).xyz;
        vec3 N = normalize( ModelView2*vec4(vNormal, 0.0) ).xyz;
        vec4 ambient = AmbientProduct;

        if (L1_Toggle == 1) {
            vec3 L1 = normalize(Light1Position.xyz - pos);
            vec3 E1 = normalize(-pos);
            vec3 H1 = normalize(L1 + E1);

            float k_d1 = max( dot(L1, N), 0.0 );
            vec4  diffuse1 = k_d1 * DiffuseProduct;

            float k_s1 = pow( max(dot(N, H1), 0.0), Shininess );
            vec4  specular1 = k_s1 * SpecularProduct;

            if(dot(L1, N) < 0.0) {
                specular1 = vec4(0.0, 0.0, 0.0, 1.0);
            } 

            light1_color = ambient + diffuse1 + specular1;
        }

        if (L2_Toggle == 1) {

            vec3 L2 = normalize(Light2Position.xyz);
            vec3 E2 = normalize(-pos);
            vec3 H2 = normalize(L2 + E2);

            float k_d2 = max(dot(L2, N), 0.0);
            vec4  diffuse2 = k_d2 * DiffuseProduct;

            float k_s2 = pow(max(dot(N, H2), 0.0), Shininess);
            vec4  specular2 = k_s2 * SpecularProduct;

            if(dot(L2, N) < 0.0) {
                specular2 = vec4(0.0, 0.0, 0.0, 1.0);
            } 

            light2_color = ambient + diffuse2 + specular2;
        }

        color = light1_color + light2_color;
        color.a = 1.0;  
    }

    if (DRAWING_MODE_GLOBAL == 2 || DRAWING_MODE_GLOBAL == 3 || Secondary_Mode == 1) {
        fN = (ModelView2*vec4(vNormal, 0.0)).xyz;
        fE = (ModelView2 * vPosition).xyz; 

        Light1 = Light1Position.xyz;
        Light2 = Light2Position.xyz;

        if (Light1Position.w != 0.0) {
            Light1 = Light1Position.xyz - fE;
        }
        if (Light2Position.w != 0.0) {
            Light2 = Light2Position.xyz - fE;
        }
    }

    if (DRAWING_MODE_GLOBAL == 4 || Secondary_Mode == 1) {
        v_coords = vCoords;
    }

    gl_Position = Projection * ModelView * vPosition;
} 
