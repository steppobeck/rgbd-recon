#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_texture_array : enable

varying vec3  pos_es;
varying vec3  pos_cs;
varying float vizvalue;

uniform int voldepth;
uniform int viztype;
uniform sampler2DArray depth_map_curr;

uniform mat4 img_to_eye_curr;
uniform vec2 viewportSizeInv;
uniform vec2 offset;

uniform float alpha_fac;


float Gamma        = 0.80;
float IntensityMax = 255.0;
	
float round(float d){
  return floor(d + 0.5);
}
	
float Adjust(float Color, float Factor){
  if (Color == 0.0){
    return 0.0;
  }
  else{
    float res = round(IntensityMax * pow(Color * Factor, Gamma));
    return min(255.0, max(0.0, res));
  }
}

vec3 WavelengthToRGB(float Wavelength){
  float Blue;
  float factor;
  float Green;
  float Red;
  if(380.0 <= Wavelength && Wavelength <= 440.0){
    Red   = -(Wavelength - 440.0) / (440.0 - 380.0);
    Green = 0.0;
    Blue  = 1.0;
  }
  else if(440.0 < Wavelength && Wavelength <= 490.0){
    Red   = 0.0;
    Green = (Wavelength - 440.0) / (490.0 - 440.0);
    Blue  = 1.0;
  }
  else if(490.0 < Wavelength && Wavelength <= 510.0){
    Red   = 0.0;
    Green = 1.0;
    Blue  = -(Wavelength - 510.0) / (510.0 - 490.0);
  }
  else if(510.0 < Wavelength && Wavelength <= 580.0){
    Red   = (Wavelength - 510.0) / (580.0 - 510.0);
    Green = 1.0;
    Blue  = 0.0;
  }
  else if(580.0 < Wavelength && Wavelength <= 645.0){		
    Red   = 1.0;
    Green = -(Wavelength - 645.0) / (645.0 - 580.0);
    Blue  = 0.0;
  }
  else if(645.0 < Wavelength && Wavelength <= 780.0){
    Red   = 1.0;
    Green = 0.0;
    Blue  = 0.0;
  }
  else{
    Red   = 0.0;
    Green = 0.0;
    Blue  = 0.0;
  }
  
  
  if(380.0 <= Wavelength && Wavelength <= 420.0){
    factor = 0.3 + 0.7*(Wavelength - 380.0) / (420.0 - 380.0);
  }
  else if(420.0 < Wavelength && Wavelength <= 701.0){
    factor = 1.0;
  }
  else if(701.0 < Wavelength && Wavelength <= 780.0){
    factor = 0.3 + 0.7*(780.0 - Wavelength) / (780.0 - 701.0);
  }
  else{
    factor = 0.0;
  }
  float R = Adjust(Red,   factor);
  float G = Adjust(Green, factor);
  float B = Adjust(Blue,  factor);
  return vec3(R/255.0,G/255.0,B/255.0);
}
	
	
	
	
float GetWaveLengthFromDataPoint(float Value, float MinValue, float MaxValue){
  float MinVisibleWavelength = 380.0;//350.0;
  float MaxVisibleWavelength = 780.0;//650.0;
  //Convert data value in the range of MinValues..MaxValues to the 
  //range 350..780
  return (Value - MinValue) / (MaxValue-MinValue) * (MaxVisibleWavelength - MinVisibleWavelength) + MinVisibleWavelength;
}	
	
	
vec3 DataPointToColor(float Value, float MinValue, float MaxValue){
  float Wavelength = GetWaveLengthFromDataPoint(Value, MinValue, MaxValue);
  return WavelengthToRGB(Wavelength);	  
}

void main(void){

  vec3  coords = vec3(gl_FragCoord.xy * viewportSizeInv, 0.0 /*here layer is always 0*/);
  float depth_curr = texture2DArray(depth_map_curr, coords).r;
  vec4  position_curr = img_to_eye_curr * vec4(gl_FragCoord.xy + vec2(0.5,0.5),depth_curr,1.0);
  vec3  position_curr_es = (position_curr / position_curr.w).xyz;
  if(length(position_curr_es) <  length(pos_es)){
    discard;
  }

  switch (viztype){
  case 1: // error3D
    gl_FragColor = vec4(DataPointToColor(vizvalue, 0.0, 1.0),vizvalue*0.5);
    break;
  case 2: // error2D
    gl_FragColor = vec4(DataPointToColor(vizvalue, 0.0, 1.0),vizvalue*0.5);
    break;
  case 3: // nnipercentage
    if(vizvalue > 0.5){
      gl_FragColor = vec4(0.0,1,0.0,0.02);
    }
    break;
  }
  
  

}

