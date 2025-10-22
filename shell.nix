{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    libGL
    glfw

    libx11
    xorg.libXrandr
    xorg.libXi
  ];
}
