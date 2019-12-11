#pragma once

/// <summary>
/// This namespace contains all enumerations used within PBRViewer.
/// </summary>
/// <remarks>
/// A static number should be assigned to each entry so that conditions and switch cases can be associated more easily in GLSL.
/// </remarks>
namespace PBRViewerEnumerations
{
	/// <summary>
	/// Entries for the diffuse term which can be selected by the user for the Cook-Torrance BRDF.
	/// </summary>
	enum DiffuseTerm
	{
		NoDiffuseTerm = 0,
		Lambertian = 1,
		LambertianEnergyConserving = 2,
		Burley = 3,
		ShirleyEtAl = 4,
		AshikhminShirleyDiffuse = 5
	};

	/// <summary>
	/// Entries for the fresnel term which can be selected by the user for the Cook-Torrance BRDF.
	/// </summary>
	enum FresnelTerm
	{
		NormalIncidence = 0,
		Schlick = 1		
	};

	/// <summary>
	/// Entries for the lighting model which can be selected by the user.
	/// </summary>
	enum LightingVariant
	{
		NoLighting = 0,
		BlinnPhong = 1,
		CookTorrance = 2,
		OrenNayar = 3,
		AshikhminShirley = 4,
		Debug = 5,
		Disney = 6
	};

	/// <summary>
	/// Entries for the normal distribution function which can be selected by the user for the Cook-Torrance BRDF.
	/// </summary>
	enum NormalDistributionTerm
	{
		ConstantValue = 0,
		TrowbridgeReitzGGX = 1,
		Beckmann = 2,
		BlinnPhongNDF = 3
	};

	/// <summary>
	/// Entries for the masking-shadowing term which can be selected by the user for the Cook-Torrance BRDF.
	/// </summary>
	enum GeometryTerm
	{
		ConstantTerm = 0,
		NoGModel = 1,
		SeparableSchlick_GGX = 2,
		SeparableSmith_GGX = 3,
		SeparableSmith_Beckmann = 4,
		SmithHeightCorrelated_GGX = 5,
		HeitzSmithHeightDirectionCorrelated_GGX = 6
	};

	/// <summary>
	/// Entries for the output on the screen.
	/// Please note: this can be overriden by the debug output.
	/// </summary>
	enum RenderOutput
	{
		Albedo = 0,
		AmbientOcclusion = 1,
		BRDFLookup = 2,
		Color = 3,
		Emissive = 4,
		Metallic = 5,		
		Roughness = 6		
	};

	/// <summary>
	/// Entries for output on the screen if the debug widget is active. 
	/// Please note: the debug output overrides the render output.
	/// </summary>
	enum DebugOutput
	{		
		NegativeNDotL = 0,
		NegativeNDotV = 1,
		NormalVectors = 2,
		TangentVectors = 3,
		BitangentVectors = 4,
		WWFT_GGX = 5
	};	

	/// <summary>
	/// Entries for the currently visible skybox texture.
	/// </summary>
	enum SkyboxTexture
	{
		Environment = 0,
		Irradiance = 1,
		PreFilteredEnvironment = 2
	};

	/// <summary>
	/// Entries for scaling.
	/// </summary>
	enum ScaleOperation
	{
		Increase = 0,
		Decrease = 1
	};
};
