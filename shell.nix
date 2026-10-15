{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    premake5
    libGL
    glfw

    libx11
    xorg.libXrandr
    xorg.libXi
  ];
}
