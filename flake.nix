{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable-small";
  };
  outputs =
    { self, nixpkgs }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "aarch64-darwin"
      ];
      forEachSystem = f: nixpkgs.lib.genAttrs systems (system: f system);
    in
    {
      devShells = forEachSystem (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          default = pkgs.mkShell {
            buildInputs = with pkgs; [
              bison
              cmake
              flex
              gnumake
            ];
            nativeBuildInputs = with pkgs; [
              direnv
              git
              yamllint
              nil
              nixfmt
            ];
          };
        }
      );
      packages = forEachSystem (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        pkgs.stdenv.mkDerivation {
          pname = "elkhound";
          version = "1.0";
          src = ./src;
          nativeBuildInputs = with pkgs; [
            bison
            cmake
            flex
            gnumake
          ];
          buildInputs = with pkgs; [
            git
          ];
          configurePhase = ''
            cmake -Wno-author -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DEXTRAS=OFF -DOCAML=OFF -B $out/build -D CMAKE_BUILD_TYPE=Release
          '';
          buildPhase = ''
            make -j$(nproc) -C $out/build
          '';
          installPhase = ''
            mkdir -p $out/bin
            cp --preserve=mode -r $out/build/elkhound/elkhound $out/bin/elkhound
            rm -rf $out/build
          '';
          enableParallelBuilding = true;
        }
      );
      formatter = forEachSystem (system: nixpkgs.${system}.nixfmt);
    };
}
