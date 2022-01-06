#version 330

in vec2 ftexcoord;
in vec3 fposition;

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

vec2 getOffset(int i, int kernelSize)
{
	int kernelCenter = int(kernelSize / 2.0f);
	
	return vec2(
		(i - (kernelSize * floor(float(i) / kernelSize)) - kernelCenter),
		(i / kernelSize - kernelCenter)
	);
}

vec3 blur(sampler2D tex, vec2 texcoord)
{
	vec2 texSize = textureSize(tex, 0);
	
	vec2 offset = vec2(1.f / texSize.x, 1.f / texSize.y);
	
	int kernelSize = 3;
	
	float kernel[9] = float[](
        1, 2, 1,
        2, 4, 2,
        1, 2, 1
    );
	
	vec3 color = vec3(0.0);

	float kernelSumm = 0;
	
	for (int i = 0; i < kernel.length(); i++) 
	{
		vec2 currentOffset = getOffset(i, kernelSize);
		color += vec3(texture(tex, texcoord.st + currentOffset * offset)) * kernel[i];
		
		kernelSumm += kernel[i];
	}

	color /= kernelSumm;
	
	return color;
}

void main()
{
	vec3 color = blur(texture1, ftexcoord);
	
	vec3 additionalColor = blur(texture2, ftexcoord);
	
	float intensivity = length(fposition) / length(vec3(1.f));
	
	FragColor = vec4(color * intensivity + additionalColor * (1.f - intensivity), 1.0);
}
