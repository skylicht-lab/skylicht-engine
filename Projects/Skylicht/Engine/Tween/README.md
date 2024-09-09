Easing functions C++
================

30 easing functions implemented in C++. <br/>
More info how functions look like and their behaviour on [easings.net](http://easings.net/).


How to use
-----
Include h-file.

    #include "easing.h"

After that there are two ways:

1. Just call one of the 30 implemented functions:
      
        easeInSine( 0.5 );

2. Or to make functions calls more unified, you can use the help-function (getEasingFunction) to get the pointer to the needed function and call it:


        auto easingFunction = getEasingFunction( EaseInExpo );
        double progress = easingFunction( 0.5 );	// 0.058

        easingFunction = getEasingFunction( EaseOutQuint );
        progress = easingFunction( 0.5 );	//0.968
  

Function that returns needed pointer has the following definition:

    typedef double(*easingFunction)(double);
    easingFunction getEasingFunction( easing_functions function );

And accept one the enums:

    enum easing_functions
    {
      EaseInSine,
    	EaseOutSine,
    	EaseInOutSine,
    	EaseInQuad,
    	EaseOutQuad,
    	EaseInOutQuad,
    	EaseInCubic,
    	EaseOutCubic,
    	EaseInOutCubic,
    	EaseInQuart,
    	EaseOutQuart,
    	EaseInOutQuart,
    	EaseInQuint,
    	EaseOutQuint,
    	EaseInOutQuint,
    	EaseInExpo,
    	EaseOutExpo,
    	EaseInOutExpo,
    	EaseInCirc,
    	EaseOutCirc,
    	EaseInOutCirc,
    	EaseInBack,
    	EaseOutBack,
    	EaseInOutBack,
    	EaseInElastic,
    	EaseOutElastic,
    	EaseInOutElastic,
    	EaseInBounce,
    	EaseOutBounce,
    	EaseInOutBounce
    };
        
