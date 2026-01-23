#version 430 core

struct Material {
    sampler2D difuso;
    sampler2D ARM;
    float brilhosidade;
};

struct Luz {
    vec3 posicao;
    vec3 ambiente;
    vec3 difuso;
    vec3 especular;
};

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

out vec4 FragColor;

uniform vec3 visaoPos;
uniform Material material;
uniform Luz luz;

void main() {
    vec3 arm = texture(material.ARM, texCoords).rgb;
    float ao = arm.r;
    float roughness = arm.g;
    float metal = arm.b;

    // componente ambiente
    vec3 ambiente = luz.ambiente * texture(material.difuso, texCoords).rgb * ao;

    // componente difuso
    vec3 normal = normalize(normal);
    vec3 luzDir = normalize(luz.posicao - fragPos);
    float dif = max(dot(normal, luzDir), 0.0);
    vec3 difuso = luz.difuso * dif * texture(material.difuso, texCoords).rgb * ao * (1 - metal);

    // componente especular
    vec3 visaoDir = normalize(visaoPos - fragPos);
    vec3 reflexoDir = reflect(-luzDir, normal);
    float espec = pow(max(dot(visaoDir, reflexoDir), 0.0), material.brilhosidade);
    vec3 especular = luz.especular * espec * (1 - roughness);

    vec3 cor = ambiente + difuso + especular;
    FragColor = vec4(cor, 1.0);
}
