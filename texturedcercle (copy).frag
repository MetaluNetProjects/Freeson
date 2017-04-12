precision highp float;

#define M_PI 3.1415926535897932384626433832795

uniform sampler2D fft_history;


uniform float spiGain/*= 0.0*/;
uniform float spiCompress;
uniform float gain /*= 1.0*/;
uniform float invert/*= 0.0*/;
uniform float smooothExt /*= 0.05*/;
uniform float smooothInt /*= 0.0*/;
uniform float rMax /*= 1.0*/;
uniform float rMin /*= 0.0*/;
uniform float compress /*= 1.0*/;
uniform float fcompress;

uniform float freqMin /*= 0.0*/;
uniform float freqMax /*= 0.92*/;

uniform float alpha_invert /*= 0.0*/;
uniform float rect;
uniform float specindex;

varying vec2 Coord;

void main (void)
{
	
	float r;
	float a;
	vec2 coord, coord2;
	vec2 polarCoord, rectCoord;
	
	coord = Coord ;
	
	coord2 = coord*2.0 - vec2(1.0);
	
	polarCoord.y = sqrt(coord2.x * coord2.x + coord2.y * coord2.y); /*r*/ 
	polarCoord.x =  abs(mod(atan(coord2.y, coord2.x)/M_PI + 4.0, 2.0) - 1.0);
	
	if(rect >= 0.0) {
		rectCoord.y = coord.x;
		rectCoord.x = mod(coord.y,1.0) * 2.0;
	} else {
		rectCoord.y = coord.y;
		rectCoord.x = mod(coord.x,1.0) * 2.0;
	}
	if(rectCoord.x > 1.0) rectCoord.x = 2.0 - rectCoord.x;
	
	r = mix(polarCoord.y, rectCoord.y, abs(rect));
		
	float index = clamp((r - rMin) / (1.0 - rMin), 0.0, 1.0);
	index = invert + (1.0 - 2.0 * invert) * (index);
	index=pow(index, compress);
	
	polarCoord.x =  abs(mod(atan(coord2.y, coord2.x)/M_PI + pow(index * spiGain, spiCompress + 1.0) + 4.0, 2.0) - 1.0);

	a = mix(polarCoord.x, rectCoord.x, abs(rect));
	
	float amp = texture2D(
			fft_history, 
			vec2(
				freqMin + pow(a, 1.0 + fcompress) * (freqMax - freqMin),
				mod(specindex + index / (gain + 1.0), 1.0)
			)
		).r;
	
	gl_FragColor = vec4(vec3(amp),1.0);
	//gl_FragColor = mix(texture2D(bottomTexture, vec2(amp, r)), texture2D(topTexture, vec2(1-amp, r)), a);
	//gl_FragColor.a *= amp;
	
	//if(alpha_invert!=0)
		gl_FragColor = alpha_invert * vec4(1.0) + (1.0 - 2.0 * alpha_invert) * gl_FragColor;
		//gl_FragColor.a = 1- gl_FragColor.a;
	
		
	
	gl_FragColor.a = gl_FragColor.a * smoothstep(0.0, smooothExt,1.0 - (r - rMin) / (rMax - rMin)) * smoothstep(0.0,smooothInt,r-rMin);

}
