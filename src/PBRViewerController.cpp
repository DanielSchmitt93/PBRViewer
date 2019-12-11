#include "PBRViewerController.h"

#include <GLFW/glfw3.h>

#include "PBRViewerOpenGLUtilities.h"
#include <experimental/filesystem>
#include "PBRViewerMouseCallbacks.h"
#include "PBRViewerKeyboardCallbacks.h"
#include "PBRViewerFramebufferCallbacks.h"

/// <summary>
/// Initializes the architectural model of the MVC pattern.
/// </summary>
GLvoid PBRViewerController::InitModel() const
{
	myModel->Init();
}

/// <summary>
/// Starts the render (game) loop.
/// </summary>
GLvoid PBRViewerController::StartRenderLoop()
{
	myLastTime = glfwGetTime();

	while (GL_FALSE == glfwWindowShouldClose(myModel->GetWindowContext()))
	{
		CalculateFps();

		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myModel->DrawOpenGL();
		myOverlayRoot->drawWidgets();

		// NanoVG, the underlying library to draw the UI parts, changes the state of the OpenGL state machine.
		// To ensure correct render calls, we reset the relevant states definded by the following link: 
		// https://github.com/memononen/nanovg#opengl-state-touched-by-the-backend		
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glfwSwapBuffers(myModel->GetWindowContext());
	}
}

/// <summary>
/// Disposes internal instances and frees memory.
/// </summary>
GLvoid PBRViewerController::Cleanup()
{
	if(myOverlayRoot)
	{			
		myOverlayRoot.reset();
	}

	if(myModel)
	{
		myModel->Cleanup();
		myModel.reset();
	}	

	glfwTerminate();	
}

/// <summary>
/// Initializes the architectural view of the MVC pattern (the UI elements).
/// </summary>
GLvoid PBRViewerController::InitOverlay()
{
	myOverlayRoot->initialize(myModel->GetWindowContext(), GL_TRUE);
	myOverlayRoot->CreateComponents();
	myOverlayRoot->setVisible(GL_TRUE);
	myOverlayRoot->performLayout();

	myOverlayRoot->MoveComponentTopLeft(myOverlayRoot->GraphicSettings);
	myOverlayRoot->MoveComponentTopRight(myOverlayRoot->ModelLoader);
	myOverlayRoot->MoveComponentBottomRight(myOverlayRoot->IBLSettings);

	ConfigureOverlayComponents();
	SetGlfwCallbacks(myModel->GetWindowContext());
}

/// <summary>
/// Sets the callbacks for the GLFW window (the window to draw into).
/// </summary>
GLvoid PBRViewerController::SetGlfwCallbacks( GLFWwindow* window ) const
{
	PBRViewerMouseCallbacks::RegisterCallbacks(window, myOverlayRoot, myModel);
	PBRViewerKeyboardCallbacks::RegisterCallbacks(window, myOverlayRoot, myModel);	
	PBRViewerFramebufferCallbacks::RegisterCallbacks(window, myOverlayRoot, myModel);	
}

/// <summary>
/// Calculates frames per second.
/// </summary>
GLvoid PBRViewerController::CalculateFps()
{
	myNumberOfFrames++;
	const GLdouble currentTime = glfwGetTime();

	// Update fps counter every second
	if (currentTime - myLastTime >= 1.0)
	{
		myOverlayRoot->ModelLoader->SetFpsCounterContent(std::to_string(myNumberOfFrames));

		myNumberOfFrames = 0;
		myLastTime += 1.0;
	}
}

/// <summary>
/// Sets the callbacks for all overlay components, i. e. the visible windows.
/// </summary>
GLvoid PBRViewerController::ConfigureOverlayComponents()
{
	ConfigureModelLoader();
	ConfigureGraphicSettings();
	ConfigureIBLSettings();
}

/// <summary>
/// Sets the callbacks for the model loader window.
/// </summary>
GLvoid PBRViewerController::ConfigureModelLoader()
{
	myOverlayRoot->ModelLoader->SetOpenButtonCallback([&]
	{
		const std::vector<std::pair<std::string, std::string>> supportedFileTypes{
			{"obj", "3D Object File"},
			{"gltf", "GL Transmission Format"}			
		};

		// nanogui changes the working directory which can lead to hard-to-find errors. Therefore we reset the working directory.
		const std::experimental::filesystem::path restorePath = std::experimental::filesystem::current_path();

		std::string resultFileDialog = nanogui::file_dialog(supportedFileTypes, GL_FALSE);
		if (resultFileDialog.empty())
		{
			return;
		}
		current_path(restorePath);

		myModel->LoadNewModel(resultFileDialog);

		// Just show the filename in the textbox, not the whole path
		const size_t last_slash_idx = resultFileDialog.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
		{
			resultFileDialog.erase(0, last_slash_idx + 1);
		}
		myOverlayRoot->ModelLoader->SetTextBoxOpenModelContent(resultFileDialog);
	});

	myOverlayRoot->ModelLoader->SetLoadSkyboxButtonCallback([&]
	{
		const std::vector<std::pair<std::string, std::string>> supportedFileTypes{		
			{"hdr", "High Dynamic Range"}
		};

		// nanogui changes the working directory which can lead to hard-to-find errors. Therefore we reset the working directory.
		const std::experimental::filesystem::path restorePath = std::experimental::filesystem::current_path();

		std::string resultFileDialog = nanogui::file_dialog(supportedFileTypes, GL_FALSE);
		if (resultFileDialog.empty())
		{
			return;
		}
		current_path(restorePath);

		myModel->LoadNewSkybox(resultFileDialog);

		// Just show the filename in the textbox, not the whole path		
		const size_t last_slash_idx = resultFileDialog.find_last_of("\\/");
		if (std::string::npos != last_slash_idx)
		{
			resultFileDialog.erase(0, last_slash_idx + 1);
		}
		myOverlayRoot->ModelLoader->SetTextBoxSkyboxContent(resultFileDialog);
	});

	myOverlayRoot->ModelLoader->SetClearModelButtonCallback([this]
	{
		myModel->ClearModel();
		myOverlayRoot->ModelLoader->SetTextBoxOpenModelContent("No model");
	});

	myOverlayRoot->ModelLoader->SetClearSkyboxButtonCallback([this]
	{
		myModel->ClearSkybox();
		myOverlayRoot->ModelLoader->SetTextBoxSkyboxContent("No skybox");
	});
}

/// <summary>
/// Sets the callbacks for the graphic settings window.
/// </summary>
GLvoid PBRViewerController::ConfigureGraphicSettings() const
{
	// Blinn/Phong
	myOverlayRoot->GraphicSettings->BlinnPhongTab->SetActivationButtonCallback([this]( const GLboolean activated )
	{
		myOverlayRoot->GraphicSettings->BlinnPhongTab->SetActivationButtonActive(activated);

		if (activated)
		{
			myModel->ChangeLighting(PBRViewerEnumerations::LightingVariant::BlinnPhong);

			// Deactivate all other tabs.
			for (auto tab : myOverlayRoot->GraphicSettings->Tabs)
			{
				if (tab != nullptr && tab != myOverlayRoot->GraphicSettings->BlinnPhongTab)
				{
					tab->SetActivationButtonActive(GL_FALSE);
				}
			}
		}
		else
		{
			myModel->ChangeLighting(PBRViewerEnumerations::NoLighting);
		}
	});

	myOverlayRoot->GraphicSettings->BlinnPhongTab->SetExponentSliderCallback([this]( const GLuint exponent )
	{
		if (myModel->GetCurrentLightingVariant() != PBRViewerEnumerations::LightingVariant::BlinnPhong)
		{
			return;
		}

		myModel->SetBlinnPhongExponent(exponent);
	});

	// Cook-Torrance
	myOverlayRoot->GraphicSettings->CookTorranceTab->SetActivationButtonCallback([this]( const GLboolean activated )
	{
		myOverlayRoot->GraphicSettings->CookTorranceTab->SetActivationButtonActive(activated);

		if (activated)
		{
			myModel->ChangeLighting(PBRViewerEnumerations::LightingVariant::CookTorrance);

			// Deactivate all other tabs.
			for (auto tab : myOverlayRoot->GraphicSettings->Tabs)
			{
				if (tab != nullptr && tab != myOverlayRoot->GraphicSettings->CookTorranceTab)
				{
					tab->SetActivationButtonActive(GL_FALSE);
				}
			}
		}
		else
		{
			myModel->ChangeLighting(PBRViewerEnumerations::LightingVariant::NoLighting);
		}
	});

	myOverlayRoot->GraphicSettings->CookTorranceTab->SetDiffuseTermComboBoxCallback(
		[this]( const PBRViewerEnumerations::DiffuseTerm currentDiffuseTerm )
		{
			if (myModel->GetCurrentLightingVariant() != PBRViewerEnumerations::LightingVariant::CookTorrance)
			{
				return;
			}

			myModel->SetDiffuseTerm(currentDiffuseTerm);
		});

	myOverlayRoot->GraphicSettings->CookTorranceTab->SetFresnelTermComboBoxCallback(
		[this]( const PBRViewerEnumerations::FresnelTerm currentFresnelTerm )
		{
			if (myModel->GetCurrentLightingVariant() != PBRViewerEnumerations::LightingVariant::CookTorrance)
			{
				return;
			}

			myModel->ChangeFresnelTerm(currentFresnelTerm);
		});

	myOverlayRoot->GraphicSettings->CookTorranceTab->SetNormalDistributionComboBoxCallback(
		[this]( const PBRViewerEnumerations::NormalDistributionTerm currentNormalDistributionTerm )
		{
			if (myModel->GetCurrentLightingVariant() != PBRViewerEnumerations::LightingVariant::CookTorrance)
			{
				return;
			}

			myModel->ChangeNormalDistributionTerm(currentNormalDistributionTerm);
		});

	myOverlayRoot->GraphicSettings->CookTorranceTab->SetGeometryTermComboBoxCallback(
		[this]( const PBRViewerEnumerations::GeometryTerm currentGeometryTerm )
		{
			if (myModel->GetCurrentLightingVariant() != PBRViewerEnumerations::LightingVariant::CookTorrance)
			{
				return;
			}

			myModel->ChangeGeometryTerm(currentGeometryTerm);
		});

	myOverlayRoot->GraphicSettings->CookTorranceTab->SetCustomMetalnessSliderCallback([this]( const GLfloat value )
	{
		if (myModel->GetCurrentLightingVariant() != PBRViewerEnumerations::LightingVariant::CookTorrance)
		{
			return;
		}

		myModel->SetCustomMetalness(value);
	});

	myOverlayRoot->GraphicSettings->CookTorranceTab->SetCustomRoughnessSliderCallback([this]( const GLfloat value )
	{
		if (myModel->GetCurrentLightingVariant() != PBRViewerEnumerations::LightingVariant::CookTorrance)
		{
			return;
		}

		myModel->SetCustomRoughness(value);
	});

	myOverlayRoot->GraphicSettings->CookTorranceTab->SetEnableCustomMaterialValuesCheckBoxCallback([this]( const GLboolean activated )
	{
		if (myModel->GetCurrentLightingVariant() != PBRViewerEnumerations::LightingVariant::CookTorrance)
		{
			return;
		}

		myModel->SetEnableCustomMaterialValues(activated);
	});

	// Oren-Nayar
	myOverlayRoot->GraphicSettings->OrenNayarTab->SetActivationButtonCallback([this]( const GLboolean activated )
	{
		myOverlayRoot->GraphicSettings->OrenNayarTab->SetActivationButtonActive(activated);

		if (activated)
		{
			myModel->ChangeLighting(PBRViewerEnumerations::OrenNayar);

			// Deactivate all other tabs.
			for (auto tab : myOverlayRoot->GraphicSettings->Tabs)
			{
				if (tab != nullptr && tab != myOverlayRoot->GraphicSettings->OrenNayarTab)
				{
					tab->SetActivationButtonActive(GL_FALSE);
				}
			}
		}
		else
		{
			myModel->ChangeLighting(PBRViewerEnumerations::NoLighting);
		}
	});

	myOverlayRoot->GraphicSettings->OrenNayarTab->SetCustomRoughnessSliderCallback([this]( const GLfloat value )
	{
		if (myModel->GetCurrentLightingVariant() != PBRViewerEnumerations::LightingVariant::OrenNayar)
		{
			return;
		}

		myModel->SetCustomRoughness(value);
	});

	myOverlayRoot->GraphicSettings->OrenNayarTab->SetEnableCustomMaterialValuesCheckBoxCallback([this]( const GLboolean activated )
	{
		if (myModel->GetCurrentLightingVariant() != PBRViewerEnumerations::LightingVariant::OrenNayar)
		{
			return;
		}

		myModel->SetEnableCustomMaterialValues(activated);
	});

	// Ashikhmin-Shirley
	myOverlayRoot->GraphicSettings->AshikhminShirleyTab->SetActivationButtonCallback([this](const GLboolean activated)
	{
		myOverlayRoot->GraphicSettings->AshikhminShirleyTab->SetActivationButtonActive(activated);

		if (activated)
		{
			myModel->ChangeLighting(PBRViewerEnumerations::AshikhminShirley);

			// Deactivate all other tabs.
			for (auto tab : myOverlayRoot->GraphicSettings->Tabs)
			{
				if (tab != nullptr && tab != myOverlayRoot->GraphicSettings->AshikhminShirleyTab)
				{
					tab->SetActivationButtonActive(GL_FALSE);
				}
			}
		}
		else
		{
			myModel->ChangeLighting(PBRViewerEnumerations::NoLighting);
		}
	});

	myOverlayRoot->GraphicSettings->AshikhminShirleyTab->SetNuSliderCallback([this](const GLuint value)
	{
		myModel->SetAshikhminShirleyNu(value);
	});

	myOverlayRoot->GraphicSettings->AshikhminShirleyTab->SetNvSliderCallback([this](const GLuint value)
	{
		myModel->SetAshikhminShirleyNv(value);
	});

	// Debug
	myOverlayRoot->GraphicSettings->DebugTab->SetActivationButtonCallback([this]( const GLboolean activated )
	{
		myOverlayRoot->GraphicSettings->DebugTab->SetActivationButtonActive(activated);

		if (activated)
		{
			myModel->ChangeLighting(PBRViewerEnumerations::LightingVariant::Debug);

			// Deactivate all other tabs.
			for (auto tab : myOverlayRoot->GraphicSettings->Tabs)
			{
				if (tab != nullptr && tab != myOverlayRoot->GraphicSettings->DebugTab)
				{
					tab->SetActivationButtonActive(GL_FALSE);
				}
			}
		}
		else
		{
			myModel->ChangeLighting(PBRViewerEnumerations::LightingVariant::NoLighting);
		}
	});

	myOverlayRoot->GraphicSettings->DebugTab->SetOutputComboBoxCallback([this]( const PBRViewerEnumerations::DebugOutput currentDebugOutput )
	{
		myModel->SetDebugOutput(currentDebugOutput);
	});

	// Disney
	myOverlayRoot->GraphicSettings->DisneyTab->SetActivationButtonCallback([this](const GLboolean activated)
	{
		myOverlayRoot->GraphicSettings->DisneyTab->SetActivationButtonActive(activated);

		if (activated)
		{
			myModel->ChangeLighting(PBRViewerEnumerations::LightingVariant::Disney);

			// Deactivate all other tabs.
			for (auto tab : myOverlayRoot->GraphicSettings->Tabs)
			{
				if (tab != nullptr && tab != myOverlayRoot->GraphicSettings->DisneyTab)
				{
					tab->SetActivationButtonActive(GL_FALSE);
				}
			}
		}
		else
		{
			myModel->ChangeLighting(PBRViewerEnumerations::LightingVariant::NoLighting);
		}
	});

	myOverlayRoot->GraphicSettings->DisneyTab->SetSubsurfaceSliderCallback([this] (const GLfloat value)
	{
		myModel->SetDisneySubsurface(value);
	});

	myOverlayRoot->GraphicSettings->DisneyTab->SetMetallicSliderCallback([this](const GLfloat value)
	{
		myModel->SetDisneyMetallic(value);
	});

	myOverlayRoot->GraphicSettings->DisneyTab->SetSpecularSliderCallback([this](const GLfloat value)
	{
		myModel->SetDisneySpecular(value);
	});

	myOverlayRoot->GraphicSettings->DisneyTab->SetSpecularTintSliderCallback([this](const GLfloat value)
	{
		myModel->SetDisneySpecularTint(value);
	});

	myOverlayRoot->GraphicSettings->DisneyTab->SetRoughnessSliderCallback([this](const GLfloat value)
	{
		myModel->SetDisneyRoughness(value);
	});

	myOverlayRoot->GraphicSettings->DisneyTab->SetAnisotropicSliderCallback([this](const GLfloat value)
	{
		myModel->SetDisneyAnisotropic(value);
	});

	myOverlayRoot->GraphicSettings->DisneyTab->SetSheenSliderCallback([this](const GLfloat value)
	{
		myModel->SetDisneySheen(value);
	});

	myOverlayRoot->GraphicSettings->DisneyTab->SetSheenTintSliderCallback([this](const GLfloat value)
	{
		myModel->SetDisneySheenTint(value);
	});

	myOverlayRoot->GraphicSettings->DisneyTab->SetClearcoatSliderCallback([this](const GLfloat value)
	{
		myModel->SetDisneyClearcoat(value);
	});

	myOverlayRoot->GraphicSettings->DisneyTab->SetClearcoatGlossSliderCallback([this](const GLfloat value)
	{
		myModel->SetDisneyClearcoatGloss(value);
	});

	// General settings
	myOverlayRoot->GraphicSettings->SetColorWheelCallback([this]( const nanogui::Color& selectedColor )
	{
		myModel->SetLightingColor(glm::vec3(selectedColor.r(), selectedColor.g(), selectedColor.b()));
	});


	myOverlayRoot->GraphicSettings->SetRenderOutputComboBoxCallback(
		[this]( const PBRViewerEnumerations::RenderOutput currentRenderOutput )
		{
			myModel->ChangeRenderOutput(currentRenderOutput);
		});

	myOverlayRoot->GraphicSettings->SetFirstLightCheckboxCallback([this]( const GLboolean activated )
	{
		if (activated)
		{
			myModel->ActivateLightSource(0);
		}
		else
		{
			myModel->DisableLightSource(0);
		}
	});

	myOverlayRoot->GraphicSettings->SetSecondLightCheckboxCallback([this]( const GLboolean activated )
	{
		if (activated)
		{
			myModel->ActivateLightSource(1);
		}
		else
		{
			myModel->DisableLightSource(1);
		}
	});

	myOverlayRoot->GraphicSettings->SetThirdLightCheckboxCallback([this]( const GLboolean activated )
	{
		if (activated)
		{
			myModel->ActivateLightSource(2);
		}
		else
		{
			myModel->DisableLightSource(2);
		}
	});

	myOverlayRoot->GraphicSettings->SetFourthLightCheckboxCallback([this]( const GLboolean activated )
	{
		if (activated)
		{
			myModel->ActivateLightSource(3);
		}
		else
		{
			myModel->DisableLightSource(3);
		}
	});

	myOverlayRoot->GraphicSettings->SetEnableShadowsCheckBoxCallback([this]( const GLboolean activated )
	{
		myModel->SetEnableShadows(activated);
	});

	myOverlayRoot->GraphicSettings->SetGammaSliderCallback([this]( const GLfloat value )
	{
		myModel->SetGamma(value);
	});

	myOverlayRoot->GraphicSettings->SetExposureSliderCallback([this]( const GLfloat value )
	{
		myModel->SetExposure(value);
	});
}

/// <summary>
/// Sets the callbacks for the Image Based Lighting window.
/// </summary>
GLvoid PBRViewerController::ConfigureIBLSettings() const
{
	myOverlayRoot->IBLSettings->SetSkyboxTextureComboBoxCallback([this]( const PBRViewerEnumerations::SkyboxTexture currentSkyboxTexture )
	{
		myModel->SetSkyboxTexture(currentSkyboxTexture);
	});

	myOverlayRoot->IBLSettings->SetMipMapLevelSliderCallback([this]( const GLuint value )
	{
		myModel->SetSkyboxTextureMipMapLevel(value);
	});
}
