#version 440

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture0;

uniform vec3 CamPos;

struct LightInfo {
    vec4 Position; // Light position in eye coords.
    vec3 Intensity; // Light intensity
};
uniform LightInfo Light[5];

struct MaterialInfo {
    vec3 Ka; // Ambient reflectivity
    vec3 Kd; // Diffuse reflectivity
    vec3 Ks; // Specular reflectivity
    float Shiness; // Specular shininess factor (phong exponent)
};
uniform MaterialInfo Material;

out vec4 FragColor;

void main() {
    vec3 base_color = texture(texture0, TexCoord).rgb;
    vec3 finalColor = vec3(0,0,0);
    vec3 ambient = Material.Ka * Light[0].Intensity;
    for (int i = 0; i < 4; i++) {
        vec3 L = normalize(Light[i].Position.xyz - Position);
        vec3 V = normalize(CamPos - Position);
        vec3 diffuse = Material.Kd * Light[i].Intensity * max( dot(L, Normal), 0.0);
        vec3 HalfwayVector = normalize(V + L);
        vec3 specular = Material.Ks * Light[i].Intensity * pow( max( dot( HalfwayVector, Normal), 0.0), Material.Shiness);
        finalColor = finalColor + diffuse + specular;
    }
    vec3 L = normalize(Light[4].Position.xyz);
    vec3 V = normalize(CamPos - Position);
    vec3 diffuse = Material.Kd * Light[4].Intensity * max( dot(L, Normal), 0.0);
    vec3 HalfwayVector = normalize(V + L);
    vec3 specular = Material.Ks * Light[4].Intensity * pow( max( dot( HalfwayVector, Normal), 0.0), Material.Shiness);
    finalColor = finalColor + diffuse + specular;
    finalColor = ambient + finalColor;

    FragColor = vec4(finalColor * base_color, 1.0);
}
