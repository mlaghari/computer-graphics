#version 130

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform sampler2D customTexture;
uniform int DRAWING_MODE_GLOBAL;
uniform int secondaryMode;
uniform int L1_Toggle;
uniform int L2_Toggle;
uniform float Shininess;

in vec4 color;
in vec2 v_coords;
in  vec3 fN;
in  vec3 fE;
in  vec3 Light1;
in  vec3 Light2;

out vec4 fColor;


void main() { 

	vec4 colorL1 = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 colorL2 = vec4(0.0, 0.0, 0.0, 1.0);

	if (DRAWING_MODE_GLOBAL == 0) {
		fColor = color;
	} else if (DRAWING_MODE_GLOBAL == 1) { 
		fColor = color;
	}else if (DRAWING_MODE_GLOBAL == 2) {
		vec3 N = normalize(fN);
		vec3 E = normalize(fE);

		vec4 ambient = AmbientProduct;

		if(L1_Toggle == 1) {
			vec3 L1 = normalize(Light1);
			vec3 R1 = normalize(2*dot(L1, N)*(N-L1));

			float k_d1 = max(dot(L1, N), 0.0);
			vec4 diffuse1 = k_d1*DiffuseProduct;

			float k_s1 = pow(max(dot(E, R1), 0.0), Shininess);
			vec4 specular1 = k_s1*SpecularProduct;

			if(dot(L1, N) < 0.0) {
				specular1 = vec4(0.0, 0.0, 0.0, 1.0);
			}

			colorL1 = ambient + diffuse1 + specular1;
		}

		if (L2_Toggle == 1) {
			vec3 L2 = normalize(Light2);
			vec3 R2 = normalize(2*dot(L2, N)*(N-L2));

			float k_d2 = max(dot(L2, N), 0.0);
			vec4 diffuse2 = k_d2*DiffuseProduct;

			float k_s2 = pow(max(dot(E, R2), 0.0), Shininess);
			vec4 specular2 = k_s2*SpecularProduct;

			if (dot(L2, N) < 0.0) {
				specular2 = vec4(0.0, 0.0, 0.0, 1.0);
			}

			colorL2 = ambient + diffuse2 + specular2;
		}

		fColor = colorL1 + colorL2;
		fColor.a = 1.0;

	} else if (DRAWING_MODE_GLOBAL == 3) {
		vec3 N = normalize(fN);
		vec3 E = normalize(fE);

		vec4 ambient = AmbientProduct;

		if (L1_Toggle == 1) {
			vec3 L1 = normalize(Light1);
			vec3 H1 = normalize( L1 + E );        

			float k_d1 = max(dot(L1, N), 0.0);
			vec4 diffuse1 = k_d1*DiffuseProduct;

			float k_s1 = pow(max(dot(N, H1), 0.0), Shininess);
			vec4 specular1 = k_s1*SpecularProduct;

			if (dot(L1, N) < 0.0) {
				specular1 = vec4(0.0, 0.0, 0.0, 1.0);
			}
			colorL1 = ambient + diffuse1 + specular1;
		}

		if (L2_Toggle == 1) {
			vec3 L2 = normalize(Light2);
			vec3 H2 = normalize( L2 + E );        

			float k_d2 = max(dot(L2, N), 0.0);
			vec4 diffuse2 = k_d2*DiffuseProduct;

			float k_s2 = pow(max(dot(N, H2), 0.0), Shininess);
			vec4 specular2 = k_s2*SpecularProduct;

			if (dot(L2, N) < 0.0) {
				specular2 = vec4(0.0, 0.0, 0.0, 1.0);
			}
			colorL2 = ambient + diffuse2 + specular2;
		}

		fColor = colorL1 + colorL2;
		fColor.a = 1.0;
	} else if (DRAWING_MODE_GLOBAL == 4) {
		fColor = texture2D( customTexture, v_coords );
	}

	if (secondaryMode == 1) {
		if (DRAWING_MODE_GLOBAL == 1) {
			fColor = 0.7*texture2D(customTexture, v_coords);
			vec4 Gcolor = 0.3*color;
			fColor = fColor + Gcolor;
			fColor.a = 1.0;
		} else if (DRAWING_MODE_GLOBAL == 2 || DRAWING_MODE_GLOBAL == 3) {
			fColor = texture2D(customTexture, v_coords);
			fColor = 0.7*fColor + 0.15*colorL1 + 0.15*colorL2;
			fColor.a = 1.0;
		} else if (DRAWING_MODE_GLOBAL == 4) {
			vec4 Gcolor = 0.3*color;
			fColor = 0.7*fColor + Gcolor;
			fColor.a = 1.0;
		}
	}
} 

