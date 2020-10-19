#include "readGravitationalAcceleration.H"

Info<< "Reading field p\n" << endl;
volScalarField p
(
    IOobject
    (
        "p",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

Info<< "Reading field U\n" << endl;
volVectorField U
(
    IOobject
    (
        "U",
        runTime.timeName(),
        mesh,
        IOobject::MUST_READ,
        IOobject::AUTO_WRITE
    ),
    mesh
);

#include "createPhi.H"


label pRefCell = 0;
scalar pRefValue = 0.0;
setRefCell(p, piso.dict(), pRefCell, pRefValue);
mesh.setFluxRequired(p.name());

singlePhaseTransportModel laminarTransport(U, phi);

dimensionedScalar rhoInfValue
(
    "rhoInf",
    dimDensity,
    laminarTransport
);

volScalarField rhoInf
(
    IOobject
    (
        "rho",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    mesh,
    rhoInfValue
);

autoPtr<incompressible::turbulenceModel> turbulence
(
    incompressible::turbulenceModel::New(U, phi, laminarTransport)
);

#include "createMRF.H"
#include "createFvOptions.H"

volScalarField mu
(
    IOobject
    (
        "mu",
        runTime.timeName(),
        mesh,
        IOobject::NO_READ,
        IOobject::AUTO_WRITE
    ),
    laminarTransport.nu()*rhoInfValue
);

word kinematicCloudName("kinematicCloud");
args.optionReadIfPresent("cloudName", kinematicCloudName);

Info<< "Constructing kinematicCloud " << kinematicCloudName << endl;
basicKinematicCollidingCloud kinematicCloud
(
    kinematicCloudName,
    rhoInf,
    U,
    mu,
    g
);

IOobject Hheader
(
    "H",
    runTime.timeName(),
    mesh,
    IOobject::MUST_READ,
    IOobject::AUTO_WRITE
);

autoPtr<volVectorField> HPtr;

if (Hheader.typeHeaderOk<volVectorField>(true))
{
    Info<< "\nReading field H\n" << endl;

    HPtr.reset(new volVectorField (Hheader, mesh));
}

IOobject HdotGradHheader
(
    "HdotGradH",
    runTime.timeName(),
    mesh,
    IOobject::MUST_READ,
    IOobject::AUTO_WRITE
);

autoPtr<volVectorField> HdotGradHPtr;

if (HdotGradHheader.typeHeaderOk<volVectorField>(true))
{
    Info<< "Reading field HdotGradH" << endl;

    HdotGradHPtr.reset(new volVectorField(HdotGradHheader, mesh));
}

#include "createNonInertialFrameFields.H"
