with import <nixpkgs> {};

stdenv.mkDerivation {
  name = "bayesian-optimisation";
  version = "1.0.0";

  buildInputs = [
    cmake
    eigen
    boost
    nlopt
    dlib
        (stdenv.mkDerivation {
      name = "bayesopt";
      src = fetchFromGitHub {
        owner = "rmcantin";
        repo = "bayesopt";
        rev = "v0.9"; # replace with the desired tag
        sha256 = "1j3bxz0c7w1hq85wycg4vv7yp6c0kzqw729y1jh8ykcygz6igwv3"; # replace with the correct sha256 hash
      };
      buildInputs = [ boost eigen cmake ];
      nativeBuildInputs = [ cmake ];
      installPhase = ''
        mkdir -p $out
        cp -r * $out
      '';
    })
  ];

  src = ./.;

  # Build phase
  buildPhase = ''
    mkdir -p build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make
  '';


}

