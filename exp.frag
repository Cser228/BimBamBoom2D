uniform vec2 screen_size;
uniform vec2 center;

void main() {
    float dist = distance(gl_FragCoord.xy/screen_size, center/screen_size);
    gl_FragColor = vec4(dist, 0, 0, 1);
}