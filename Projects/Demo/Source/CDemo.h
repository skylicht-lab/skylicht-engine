#include "IApplicationEventReceiver.h"

class CDemo : public IApplicationEventReceiver
{
public:
	CDemo();
	virtual ~CDemo();

	virtual void onUpdate();

	virtual void onRender();

	virtual void onPostRender();

	virtual void onResume();

	virtual void onPause();

	virtual void onInitApp();

	virtual void onQuitApp();

	virtual bool onBack();
};