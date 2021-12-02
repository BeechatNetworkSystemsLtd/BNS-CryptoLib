find_package(Java)
find_package(Java COMPONENTS Development)

if(MSVC)
  add_compile_options(/I${_JAVA_HOME}\\include /I${_JAVA_HOME}\\include\\win32)
  add_compile_options(/nologo /O2 /W4 /wd4146 /wd4244)
else()
  if(MSYS OR MINGW)
    add_compile_options(-I${_JAVA_HOME}\\include -I${_JAVA_HOME}\\include\\win32)
  else()
    add_compile_options(-I${_JAVA_HOME}/include -I${_JAVA_HOME}/include/linux)
  endif()
  add_compile_options(-Wall -fPIC -Wextra -Wpedantic -Werror -Wno-array-bounds
    -Wno-unused-function)
  add_compile_options(-Wshadow -Wpointer-arith)
  add_compile_options(-Wno-missing-braces -Wno-missing-field-initializers
-Wno-unused-parameter)
  add_compile_options(-O3 -fomit-frame-pointer)
endif()

