workspace("game")
   configurations { "debug", "release" }

project("game")
   kind("ConsoleApp")
   language("C++")
   cppdialect("C++20")
   targetdir("bin/%{cfg.buildcfg}")

   files { "**.hpp", "**.cpp" }

   linkbuildoutputs "On"

   filter("configurations:debug")
      defines { "DEBUG" }
      symbols("On")

   filter("configurations:release")
      defines { "NDEBUG" }
      optimize("On")
