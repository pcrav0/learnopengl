function join(a, b)
   local result = {}

   for _, v in ipairs(a) do
      table.insert(result, v)
   end

   for _, v in ipairs(b) do
      table.insert(result, v)
   end

   return result
end

function sh(cmd)
   return io.popen(cmd):read('*a')
end

function main_setup(extra_files)
   assert(#extra_files > 0)

   kind('ConsoleApp')
   language('C++')
   cppdialect('C++23')
   targetdir('bin/%{cfg.buildcfg}')
   toolset('clang')

   linkbuildoutputs('On')

   libdirs { 'obj/%{cfg.buildcfg}' }

   links{
      'utils', 'graphics',
      'glad', 'stb_image',
      'imgui',
   }
   linkoptions{
      '-lGL',
      '-lglfw',

      '-lX11',
      '-lXrandr',
      '-lXi',

      '-lpthread',
      '-ldl',
   }

   --files(join(extra_files, {'utils.hpp','utils.cpp','graphics.hpp','graphics.cpp'}))
   files(extra_files)

   filter('files:vertex_shaders.glsl') do
      buildinputs{ 'vertex_shaders.glsl' }
      buildaction('FxCompile')
   end

   filter{ 'system:linux', 'action:gmake' } do
      buildoptions{ '-pipe' }
   end

   filter('configurations:debug') do
      -- buildoptions{ '-fsanitize=memory', '-fno-omit-frame-pointer', '-fno-optimize-sibling-calls' }
      -- linkoptions { sh('pkg-config --libs fmt') }
      defines{ 'DEBUG=true' }
      symbols('On')
   end

   filter('configurations:release') do
      buildoptions{ '-fno-exceptions' }
      defines{ 'DEBUG=false', 'RELEASE', 'UTILS_ASSERT=UTILS_IGNORE' }
      optimize('On')
   end
end

workspace('game') do
   configurations{ 'debug', 'release' }
   includedirs {
      './thirdparty/stb_image/',
      './thirdparty/glad/include',
      './thirdparty/glm/',
      './thirdparty/imgui/',
      './thirdparty/imgui/backends/',
   }

   project('color') do
      main_setup{'./main-color.cpp'}
   end

   project('cord') do
      main_setup{'./main-cord.cpp'}
   end

   project('texture') do
      main_setup{'./main-texture.cpp'}
   end

   project('transformation') do
      main_setup{'./main-transformation.cpp'}
   end

   project('cam') do
      main_setup{'./main-cam.cpp'}
   end

   project('utils') do
      kind('SharedLib')
      language('C++')
      cppdialect('C++23')
      targetdir('obj/%{cfg.buildcfg}')
      toolset('clang')

      files{ 'utils.hpp', 'utils.cpp' }

      filter('configurations:debug') do
         defines{ 'DEBUG=true' }
         symbols('On')
      end

      filter('configurations:release') do
         defines{ 'DEBUG=false', 'RELEASE' }
         optimize('On')
      end
   end

   project('graphics') do
      kind('SharedLib')
      language('C++')
      cppdialect('C++23')
      targetdir('obj/%{cfg.buildcfg}')
      toolset('clang')

      files{ 'graphics.hpp', 'graphics.cpp' }

      filter('configurations:debug') do
         defines{ 'DEBUG=true' }
         symbols('On')
      end

      filter('configurations:release') do
         defines{ 'DEBUG=false', 'RELEASE' }
         optimize('On')
      end
   end

   project('glad') do
      local loc = './thirdparty/glad/'
      location(loc)

      kind('StaticLib')
      language('C')
      targetdir('obj/%{cfg.buildcfg}')
      toolset('gcc')

      files{ loc..'/src/glad.c' }

      filter('configurations:debug') do
         defines{ 'DEBUG=true' }
         symbols('On')
      end

      filter('configurations:release') do
         defines{ 'DEBUG=false', 'RELEASE' }
         optimize('On')
      end
   end

   project('stb_image') do
      local loc = './thirdparty/stb_image/'
      location(loc)

      kind('StaticLib')
      language('C')
      targetdir('obj/%{cfg.buildcfg}')
      toolset('gcc')

      files{ loc..'/stb_image.c' }

      filter('configurations:debug') do
         defines{ 'DEBUG=true' }
         symbols('On')
      end

      filter('configurations:release') do
         defines{ 'DEBUG=false', 'RELEASE' }
         optimize('On')
      end
   end

   project('imgui') do
      local loc = './thirdparty/imgui/'
      location(loc)

      kind('StaticLib')
      language('C++')
      cppdialect('C++23')
      targetdir('obj/%{cfg.buildcfg}')
      toolset('clang')

      includedirs { loc }

      files{
         loc..'/imgui.cpp',
         loc..'/imgui_draw.cpp',
         loc..'/imgui_widgets.cpp',
         loc..'/imgui_demo.cpp',
         loc..'/imgui_tables.cpp',
         loc..'/backends/imgui_impl_glfw.cpp',
         loc..'/backends/imgui_impl_opengl3.cpp',
      }

      filter('configurations:debug') do
         defines{ 'DEBUG=true' }
         symbols('On')
      end

      filter('configurations:release') do
         defines{ 'DEBUG=false', 'RELEASE' }
         optimize('On')
      end
   end
end

workspace('test') do
   configurations{ 'test' }

   project('test') do
      kind('ConsoleApp')
      language('C++')
      cppdialect('C++23')
      targetdir('bin/%{cfg.buildcfg}')
      toolset('clang')
      linkbuildoutputs('On')

      files{ 'test1.cpp', 'utils.hpp', }

      filter('configurations:test') do
         defines{ 'DEBUG' }
         symbols('On')
      end
   end
end
