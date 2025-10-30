function sh(cmd)
   return io.popen(cmd):read('*a')
end

workspace('game') do
   configurations{ 'debug', 'release' }
   includedirs { './thirdparty/stb_image/', './thirdparty/glad/include' }

   project('main') do
      kind('ConsoleApp')
      language('C++')
      cppdialect('C++23')
      targetdir('bin/%{cfg.buildcfg}')
      toolset('clang')

      linkbuildoutputs('On')

      libdirs { 'obj/%{cfg.buildcfg}' }

      links{ 'glad', 'stb_image' }
      linkoptions{
         '-lGL',
         '-lglfw',

         '-lX11',
         '-lXrandr',
         '-lXi',

         '-lpthread',
         '-ldl',
      }

      files{
         'utils.hpp', 'utils.cpp',
         'graphics.hpp', 'graphics.cpp',
         'main.cpp', 'texture-main.cpp', 'transformation-main.cpp',
      }

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

      files{ 'test.cpp', 'utils.hpp', }

      filter('configurations:test') do
         defines{ 'DEBUG' }
         symbols('On')
      end
   end
end
