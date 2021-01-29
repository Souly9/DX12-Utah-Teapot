#pragma once

class Window;
class Renderer;

/**
 * Base class for all applications
 * Used to streamline the DirectX12 process somewhat
 */
class Program : public std::enable_shared_from_this<Program>
{
public:
	Program(const std::wstring& name, int width, int height, bool vSync);
	virtual ~Program();

	/**
	* Initialize the DirectX Runtime.
	*/
	virtual bool Initialize();

	/**
	* Getter and Setter for screen stuff
	*/
	int GetWidth() const {return m_width;}
	int GetHeight() const {return m_height;}
	
	/**
	 * Load content required for the demo.
	 */
	virtual bool LoadContent() = 0;

	/**
	 * Central Update handler called in the window main loop
	 */
	virtual void OnUpdate(float deltaTime, float totalTime);

	virtual void OnRender(float deltaTime, float totalTime);

	/**
	 *  Unload demo specific content that was loaded in LoadContent.
	 */
	virtual void UnloadContent() = 0;

	/**
	 * Destroy any resource that are used by the game.
	 */
	virtual void Destroy();
	virtual void OnKeyReleased(WPARAM uint);
	virtual void OnKeyPressed(WPARAM uint);
	virtual void OnMouseWheel(float zDelta);
	virtual void OnMouseButtonPressed(int x, int y);
	virtual void OnMouseMoved(int x, int y);
	virtual void OnMouseButtonReleased(int x, int y);

protected:
    friend class Window;

	

	virtual void OnWindowDestroy();

	std::shared_ptr<Window> m_pWindow;
	float m_pitch;
	int m_width;
	int m_height;
	//Renderer m_renderer;
	std::wstring m_name;
	
	bool m_vSync;
private:
	
	
};
