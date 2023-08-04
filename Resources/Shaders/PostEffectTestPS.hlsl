#include"PostEffectTest.hlsli"


Texture2D<float4> tex0 : register(t0);  	// 0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1);  	// 1番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      	// 0番スロットに設定されたサンプラー

float WhiteNoise(float2 coord) {
	return frac(sin(dot(coord, float2(8.7819, 3.255))) * 437.645);
}


float4 main(VSOutput input) : SV_TARGET
{

	//ノイズ
	//それっぽいノイズ
	//タイム利用して常に動くノイズをできればなおよい
	//float2 samplePoint = input.uv;
	//float4 Tex = tex.Sample(smp, input.uv);
	//float noise = WhiteNoise(input.uv * 50) - 0.5;
	//Tex.rgb += float3(noise, noise, noise);
	//return float4(Tex.rgb, 1);

	////uv指定したピクセルの色をサンプリング
	//float4 texcolor = tex0.Sample(smp,input.uv);
	////RGBの数値を二倍にして出力
	//return float4(texcolor.rgb, 1);

	//分割して出す
	/*float4 colortex0 = tex0.Sample(smp,input.uv);
	float4 colortex1 = tex1.Sample(smp,input.uv);
	float4 color = colortex0;
		if (fmod(input.uv.y, 0.1f) < 0.05f)
		{
			color = colortex1;
		}
	return float4(color.rgb, 1);*/


	float2 add = { 0.5f, 0.0f };
	float4 texcolor0 = tex0.Sample(smp, input.uv);

	float offSetU = 1 / 1280.0f;
	float offSetV = 1 / 720.0f;

	int size = 2;

	float4 sum = float4(0, 0, 0, 0);


	for (int x = -size; x <= size; ++x)
	{
		for (int y = -size; y <= size; ++y)
		{
			float2 offSet = float2(x, y) * float2(offSetU, offSetV);
			float4 texcolor0 = tex0.Sample(smp, input.uv + offSet);
			sum += texcolor0;

		}

	}

	float kernelArea = (2 * size + 1) * (2 * size + 1);
	float4 averageColor = sum / kernelArea;
	float4 texcolor1 = tex1.Sample(smp, input.uv);

	float4 color = averageColor;
	if (fmod(input.uv.y, 0.1f) < 0.05f)
	{
		color = 1 - texcolor0;
	}

	return float4(color.rgb, 1);

}

//Texture2D<float4> tex : register(t0);  	// 0番スロットに設定されたテクスチャ
//smpState smp : register(s0);      	// 0番スロットに設定されたサンプラー
//
//float3 sampleMain(float2 uv) {
//    return tex.Sample(smp, uv).rgb;
//}
//
//float3 sampleBox(float2 uv, float delta) {
//    float4 _MainTex_TexelSize = float4(0, 1, 0, 0);
//
//    float4 offset = _MainTex_TexelSize.xyxy * float2(-delta, delta).xxyy;
//    float3 sum = sampleMain(uv + offset.xy) + sampleMain(uv + offset.zy) + sampleMain(uv + offset.xw) + sampleMain(uv + offset.zw);
//    return sum * 0.25;
//}
//
//float4 main(VSOutput input) : SV_TARGET
//{
//    float _Intensity = 1;
//
//       float4 col = tex.Sample(smp, input.uv);
//                col.rgb += sampleBox(input.uv, 0.5) * _Intensity;
//                return col;
//}

/*float offsetU = 10/ 1280.0f;
	float offsetV = 10 / 720.0f;


	float4 color = tex.Sample(smp, input.uv + float2(offsetU,0.0f));

	color += tex.Sample(smp, input.uv + float2(-offsetU, 0.0f));

	color += tex.Sample(smp, input.uv + float2(0.0f,offsetV));

	color += tex.Sample(smp, input.uv + float2(0.0f, -offsetV));

	color += tex.Sample(smp, input.uv + float2(offsetU, offsetV));
	color += tex.Sample(smp, input.uv + float2(-offsetU, offsetV));
	color += tex.Sample(smp, input.uv + float2(offsetU, -offsetV));
	color += tex.Sample(smp, input.uv + float2(-offsetU, -offsetV));

	color /= 8.0f;

	color+= tex.Sample(smp, input.uv);

	return float4(color);*/