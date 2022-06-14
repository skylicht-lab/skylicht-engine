# Skylicht Engine
## Git flow
### Feature
-   If you commit code for feature, open an issue ex: #282. Create your branch name: **`feature/#282-your-branch-name`** (All characters is lowercase and character '-' instead space ' ')
-   Commit message: **`#282 To do some thing`**
-   If your commit message too long
```
#282 To do some thing
Do some thing 1
Do some thing 2
...
```
### Bug
-   Open an issue ex: #283. Create your branch name: **`bug/#283-your-bug-name`**
-   Commit message: **`#283 To do fix some thing`**

Remember tag your issues id ex: #282 on your commit message, and branch name
## Code style
-   We try to adhere to the existing [**Irrlicht code style**](http://irrlicht.sourceforge.net/?page_id=140).
-   You can use your code style in your local project `Projects/{YourProject}`. If you fix bug or commit a feature on `Projects/Skylicht/*`, please follow our style.
### Code formation
Use **TAB Indent** instead 2 space for C/C++ code.

-   On Visual Studio, try format your code by hot key:
    -   **`Ctrl` + `K` + `D`** for document
    -   **`Ctrl` + `K` + `F`** for selected code

-   On Visual Code:
    -   On Windows: **`Shift` + `Alt` + `F`**
    -   On Mac: **`Shift` + `Option` + `F`**
    -   On Ubuntu: **`Ctrl` + `Shift` + `I`**
### Example code style
**`NOT RECOMMENDED JS STYLE`**
```
void function(int foo){
  ...
  if (condition){
  }
  ...  
}
```
**`GOOD`**
```
// Use hot key `Ctrl` + `K` + `D` (Visual Studio) to format this style
void function(int foo)
{
  ...
  if (condition)
  {
    ...
  }
}
```
**`NOT RECOMMENDED FOR OLD C CODE STYLE IN CLASS NAME, MEMBER FUNCTION AND VARIABLE`**

I recommend we don't use ~~c_style_function_name~~ and ~~character _~~ for function name and variable name in .cpp source

**`BAD`**
```
// not recommend underscore C style in C++ (*.cpp)
void func_style_c_name()
{
  // not recommend
  int _variable = 0;
  
  // not recommend
  int variable_ugly = 0;
}

// not recommend in .cpp
void MODULE_func_style_c_name()
{
  
}
```
**`GOOD`**
```
void camelCaseStyle()
{  
  int variable = 0;
  ...
}
```
-   **camelCase** for *`all variables and functions`*
```
void functionName(int foo)
{
  int variableName = 0;
  ..
}
```
-   Prefix **C** for *`class`*, Prefix **E** for *`enum`*, Prefix **S** for *`struct`*, Prefix **I** for *`abstract class`*
```
class IInterfaceExample
{
  ...
};

class CExampleClass
{
  ...
};

struct SExampleStruct
{
  ...
};

enum EExamleEnum
{
  ...
};
```
-   **PascalCase** for *`public variable`*
```
struct SExamleEnum
{
  int VariableA;
  int VariableB;
  int VariableC;
};

enum EExamleEnum
{
  EnumA,
  EnumB,
  EnumC,
};
```
-   **m_camelCase** for *`private/protected member variable`*
-   **g_camelCase** for *`global member`*
-   **s_camelCase** for *`static member`*
```
int g_exampleGlobalVariable;

class CExampleClass
{
  public:
    static int s_staticVariable;
    
  private:
    int m_privateVariableA;
    int m_privateVariableB;
	
  public:
    std::string PublicPropertyA;
    std::string PublicPropertyB;
	
  public:
    CExampleClass();
    
    virtual ~CExampleClass();
    
    public int getVariableA()
    {
      return m_privateVariableA;
    }
    
    public int getVariableA()
    {
      return m_privateVariableB;
    }
    ...
};
```
-   **UPPER_CASE** for *`define`*
```
#define YOUR_VARIABLE 10
#define MIN(x, y) (x < y ? x : y)
```
-   **PascalCase** for *`const`*
```
function exampleFunction()
{
  const int ExampleConst = 10;
  ...
}
```
Thanks for contribute

Skylicht Engine Author
