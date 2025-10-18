workspace("game") do
   configurations{ "debug", "release" }
end

project("game") do
   kind("ConsoleApp")
   language("C++")
   cppdialect("C++20")
   targetdir("bin/%{cfg.buildcfg}")
   toolset("clang")

   files{ "**.hpp", "**.cpp" }

   linkbuildoutputs("On")

   filter{ "system:linux", "action:gmake" } do
      buildoptions{ '-pipe' }
      --buildoptions { "$$NIX_LDFLAGS", "-ansi", "-pedantic" }
      --buildoptions { sh("pkg-config --cflags fmt"), '-pipe' }
      --linkoptions { sh("pkg-config --libs fmt") }
   end


   filter("configurations:debug") do
      defines{ "DEBUG" }
      symbols("On")
   end

   filter("configurations:release") do
      buildoptions{ '-fno-exceptions' }
      defines{ "RELEASE", "UTILS_ASSERT=UTILS_IGNORE" }
      optimize("On")
   end
end
