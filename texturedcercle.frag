precision highp float;

#define M_PI 3.1415926535897932384626433832795

//#define DORECT

uniform sampler2D fft_history;
uniform sampler2D histTex;


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
#ifdef DORECT
uniform float rect;
#endif
uniform float specindex;
uniform float lowGainR;
uniform float lowGainA;
//uniform float hiGainR;
//uniform float hiGainA;
uniform float lowGainB;

varying vec2 coord;


void main (void)
{
	
	float r;
	float a;
	vec2 coord2;
	vec2 polarCoord, rectCoord;
	
	coord2 = coord * 2.0 - vec2(1.0);
	
#ifdef DORECT
	polarCoord.y = length(coord2);

	if(rect >= 0.0) {
		rectCoord.y = coord.x;
		rectCoord.x = mod(coord.y,1.0) * 2.0;
	} else {
		rectCoord.y = coord.y;
		rectCoord.x = mod(coord.x,1.0) * 2.0;
	}
	if(rectCoord.x > 1.0) rectCoord.x = 2.0 - rectCoord.x;
	
	r = mix(polarCoord.y, rectCoord.y, abs(rect));
#else
	r = length(coord2);
#endif


	float lowLevel = texture2D(histTex, vec2(mod(r * 0.42 + specindex, 1.0), 0.0)).r;
	//float hiLevel = texture2D(histTex, vec2(mod(r * 0.35 + specindex, 1.0), 1.0)).r;
#define hiLevel 0	

#if 0
	float index = clamp((r - rMin) / (1.0 - rMin), 0.0, 1.0);
	index = invert + (1.0 - 2.0 * invert) * (index);
	index = pow(index, compress);
#else
	float index = pow(invert + (1.0 - 2.0 * invert) * r, compress);
#endif

	
#ifdef DORECT
	polarCoord.x = 
		abs(mod(atan(coord2.y, coord2.x)/M_PI + pow(index * spiGain, spiCompress + 1.0) + 4.0 + lowLevel * lowGainA, 2.0) - 1.0);

	a = mix(polarCoord.x, rectCoord.x, abs(rect));
	//a = polarCoord.x;
#else
	a = abs(mod(atan(coord2.y, coord2.x)/M_PI + pow(index * spiGain, spiCompress + 1.0) + 4.0 + lowLevel * lowGainA, 2.0) - 1.0);	
#endif
	
	float amp = texture2D(
			fft_history, 
			vec2(
				freqMin + pow(mod(a , 1.0), 1.0 + fcompress) * (freqMax - freqMin),
				mod(specindex + (clamp(index + lowLevel * lowGainR, 0.0, 1.0)) / (gain + 1.0), 1.0)
			)
		).r;
	
	gl_FragColor = vec4(vec3(amp),1.0);
	//gl_FragColor = mix(texture2D(bottomTexture, vec2(amp, r)), texture2D(topTexture, vec2(1-amp, r)), a);
	//gl_FragColor.a *= amp;
	
	//if(alpha_invert!=0)
	//gl_FragColor = alpha_invert * vec4(1.0) + (1.0 - 2.0 * alpha_invert) * gl_FragColor;
	
	float lowCol = lowGainB * lowLevel;
	
	gl_FragColor *= vec4(1.0 /*- lowGainB*/ + lowCol, 1.0 - lowCol, 1.0 - lowCol, 1.0);
		//gl_FragColor.a = 1- gl_FragColor.a;
	
		
	
	gl_FragColor.a = gl_FragColor.a * smoothstep(0.0, smooothExt,1.0 - (r - rMin) / (rMax - rMin)) * smoothstep(0.0,smooothInt,r-rMin);

}
