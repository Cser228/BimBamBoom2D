uniform float time;
uniform vec2 positionCenter;
uniform float maxSize;

void main() {
    float blink = sin(time * 25.12) * 0.5 + 0.5;
    float sizeProgress = 1.0 - time;
    float currentSize = maxSize * sizeProgress;
    float fadeOut = 1.0 - smoothstep(0.7, 1.0, time);
    
    vec2 uv = (gl_FragCoord.xy - positionCenter) / currentSize;
    float dist = length(uv);
    
    if (dist > 1.0) {
        discard;
    }
    
    vec3 red = vec3(1.0, 0.0, 0.0);
    vec3 yellow = vec3(1.0, 1.0, 0.0);
    vec3 color = mix(red, yellow, blink);
    float alpha = (1.0 - dist) * fadeOut;
    
    gl_FragColor = vec4(color, alpha);
}