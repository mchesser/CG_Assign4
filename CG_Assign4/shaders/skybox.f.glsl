#version 150

in vec2 texCoord;

out vec4 fColor;

uniform sampler2D day_texture;
uniform sampler2D night_texture;
uniform sampler2D sunset_texture;
uniform vec3 sun_position;


void main() {
    if (sun_position.y < 0.0) {
        fColor = texture(night_texture, texCoord);

    } else if (sun_position.y < 100.0) {
        vec4 night_color = texture(night_texture, texCoord);
        vec4 sunset_color = texture(sunset_texture, texCoord);
        fColor = mix(night_color, sunset_color, sun_position.y / 100.0);

    } else if (sun_position.y < 200.0) {
        vec4 sunset_color = texture(sunset_texture, texCoord);
        vec4 day_color = texture(day_texture, texCoord);
        fColor = mix(sunset_color, day_color, (sun_position.y - 100) / 100.0);
        
    } else {
        fColor = texture(day_texture, texCoord);
    }
}
