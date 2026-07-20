// carlo_vtk.cpp
// Demo VTK: cubo 3D giratorio con animacion por timer.
//
// Compilar con CMake:
//   mkdir build_vtk && cd build_vtk
//   cmake ..
//   make
//   ./carlo_vtk

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkCubeSource.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

// ── Callback de animacion: rota el cubo en cada tick del timer ────────────
/**
 * @brief Callback que se ejecuta en cada evento de timer para rotar el cubo.
 *
 * Hereda de vtkCommand. VTK llama a Execute() automaticamente cada vez
 * que el interactor genera un TimerEvent.
 */
class RotationCallback : public vtkCommand {
public:
    /// Factory method requerido por VTK para instanciar callbacks
    static RotationCallback *New() { return new RotationCallback; }

    vtkActor *Actor  = nullptr; ///< Actor del cubo a rotar
    double    AngleX = 0.5;     ///< Grados por tick en eje X
    double    AngleY = 1.0;     ///< Grados por tick en eje Y

    /**
     * @brief Se llama en cada TimerEvent. Aplica la rotacion y redibuja.
     * @param caller El interactor que genero el evento.
     */
    void Execute(vtkObject *caller, unsigned long, void *) override {
        // Rota el actor en X e Y para dar movimiento diagonal
        Actor->RotateX(AngleX);
        Actor->RotateY(AngleY);

        // Fuerza el redibujado de la ventana
        auto *iren = static_cast<vtkRenderWindowInteractor *>(caller);
        iren->GetRenderWindow()->Render();
    }
};

// ── Main ──────────────────────────────────────────────────────────────────
int main() {
    // Paleta de colores por nombre
    auto colors = vtkSmartPointer<vtkNamedColors>::New();

    // ── 1. Geometria: cubo unitario centrado en el origen ─────────────────
    auto cube = vtkSmartPointer<vtkCubeSource>::New();
    cube->SetXLength(1.5);
    cube->SetYLength(1.5);
    cube->SetZLength(1.5);

    // ── 2. Mapper: conecta la geometria al pipeline de renderizado ─────────
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(cube->GetOutputPort());

    // ── 3. Actor: representa el objeto en la escena con sus propiedades ────
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor3d("SteelBlue").GetData());
    actor->GetProperty()->SetEdgeVisibility(true);       // muestra las aristas
    actor->GetProperty()->SetEdgeColor(colors->GetColor3d("White").GetData());
    actor->GetProperty()->SetLineWidth(1.5);
    actor->GetProperty()->SetOpacity(0.85);              // ligera transparencia

    // ── 4. Texto en pantalla ───────────────────────────────────────────────
    auto textActor = vtkSmartPointer<vtkTextActor>::New();
    textActor->SetInput("Carlo VTK Demo — MatrixCarlo");
    textActor->GetTextProperty()->SetFontSize(18);
    textActor->GetTextProperty()->SetColor(colors->GetColor3d("White").GetData());
    textActor->SetPosition(20, 20);

    // ── 5. Renderer: escena con fondo degradado ────────────────────────────
    auto renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);
    renderer->AddViewProp(textActor);
    renderer->SetBackground(colors->GetColor3d("MidnightBlue").GetData());
    renderer->SetBackground2(colors->GetColor3d("Black").GetData());
    renderer->SetGradientBackground(true);               // fondo degradado
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(30);
    renderer->GetActiveCamera()->Elevation(20);

    // ── 6. Ventana de render ───────────────────────────────────────────────
    auto window = vtkSmartPointer<vtkRenderWindow>::New();
    window->AddRenderer(renderer);
    window->SetSize(800, 600);
    window->SetWindowName("Carlo VTK — Cubo Giratorio");

    // ── 7. Interactor: maneja eventos de mouse/teclado y el timer ─────────
    auto interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    interactor->SetRenderWindow(window);

    // Estilo trackball: el usuario puede rotar/zoom con el mouse
    auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    interactor->SetInteractorStyle(style);

    // ── 8. Registrar el callback de rotacion al evento de timer ───────────
    auto callback = vtkSmartPointer<RotationCallback>::New();
    callback->Actor = actor;
    interactor->AddObserver(vtkCommand::TimerEvent, callback);

    // ── 9. Arrancar: inicializar, crear timer repetitivo y entrar al loop ──
    interactor->Initialize();
    interactor->CreateRepeatingTimer(16);  // ~60 fps (16ms por frame)
    interactor->Start();

    return 0;
}
