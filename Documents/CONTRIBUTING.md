# Skylicht Engine
## Git flow
### Feature
-   If you commit code for feature, open an issue ex: #282. Create your brance name: **`feature/#282-your-brance-name`** (All characters is lowercase and - instead space)
-   Commit message: **`feat: #282 To do some thing`**
-   If your commit message too long
```
feat: #282 To do some thing
Do some thing 1
Do some thing 2
...
```
### Bug
-   Open an issue ex: #283, so your brance name: **`bug/#283-your-bug-name`**
-   Commit message: **`fix: #282 To do fix some thing`**

Remember tag your issues id ex: #282 on your commit message, and brance name
## Code style
-   We try to adhere to the existing [**Irrlicht code style**](http://irrlicht.sourceforge.net/?page_id=140)
-   You can use your code style in your local project `Projects/{YourProject}`. But if you fix bug or commit a feature on `Projects/Skylicht*`, try to follow our style.
### Code formation
Use **TAB Indent** instead 2 space for C/C++ code.
-   On Visual Studio, try format your code document by hot key: **`Ctrl` + `K` + `D`** or **`Ctrl` + `K` + `F`**
-   On Visual Code:
    -   On Windows: **`Shift` + `Alt` + `F`**
    -   On Mac: **`Shift` + `Option` + `F`**
    -   On Ubuntu: **`Ctrl` + `Shift` + `I`**
### Example code style
**`NOT RECOMMENDED`**
```
void function(int foo){
  ...
}
```
**`GOOD`**
```
void function(int foo)
{
  ...
}
```
**`Not recommended for old C Style`**
Not recommend to use ~~c_style_function_name~~ and character _ for function name and variable name

**`BAD`**
```
// ugly code
void func_style_c_name()
{
  // ugly code
  int _variable = 0;
}

// ugly code
void MODULE_func_style_c_name()
{
  
}
```
**`GOOD`**
```
void funcStyleJavaName()
{  
  int variable = 0;
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
-   **C** for *`class`*, **E** for *`enum`*, **S** for *`struct`*
```
class CExampleClass(int foo)
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

class CExampleClass(int foo)
{
  public:
    static int s_staticVariable;
    
  private:
    int m_privateVariableA;
    int m_privateVariableB;
    
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
