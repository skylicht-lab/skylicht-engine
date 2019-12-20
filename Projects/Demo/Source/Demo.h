#include "IApplicationEventReceiver.h"

class Demo : public IApplicationEventReceiver
{
public:
	Demo();
	virtual ~Demo();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual void onInitApp();

	virtual void onQuitApp();
};