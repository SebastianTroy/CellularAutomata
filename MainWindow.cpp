#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "NeuralNetwork.h"
#include "Neighbourhood.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , timer_(new QTimer(this))
{
    ui->setupUi(this);

    SetupSpeedControlls();
    SetupColourControlls();
    SetupRulesControlls();
    SetupRandomiserControlls();
    SetupCellsControlls();
    // Do last so all other settings are applied before the sim starts
    SetupTimer();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetupTimer()
{
    connect(timer_, &QTimer::timeout, [&]()
    {
        ui->cellularAutomata->Step();
    });
    timer_->setSingleShot(false);
    timer_->start();
}

void MainWindow::SetupSpeedControlls()
{
    ui->speedCustomSpinner->setMinimum(1);

    connect(ui->speed1Hz, &QRadioButton::toggled, [&](bool checked) { if (checked) timer_->setInterval(1000 / 1); });
    connect(ui->speed5Hz, &QRadioButton::toggled, [&](bool checked) { if (checked) timer_->setInterval(1000 / 5); });
    connect(ui->speedMax, &QRadioButton::toggled, [&](bool checked) { if (checked) timer_->setInterval(0); });
    connect(ui->speedCustom, &QRadioButton::toggled, [&](bool checked) { if (checked) timer_->setInterval(1000 / ui->speedCustomSpinner->value()); });
    connect(ui->speedPaused, &QCheckBox::toggled, [&](bool checked) { checked ? timer_->stop() : timer_->start(); });
    connect(ui->speedStepOnce, &QPushButton::pressed, [&]() { ui->cellularAutomata->Step(); });
    connect(ui->speedCustomSpinner, qOverload<int>(&QSpinBox::valueChanged), [&](int) { if (ui->speedCustom->isChecked()) timer_->setInterval(1000 / ui->speedCustomSpinner->value()); });

    ui->speed5Hz->setChecked(true);
}

void MainWindow::SetupColourControlls()
{
    // TODO add ColourPicker widget etc allow custom gradients/stops

    auto& ca = *ui->cellularAutomata;
    connect(ui->colourMonochrome, &QRadioButton::toggled, [&](bool checked)
    {
        if (checked) {
            ca.SetCellColouriser(ca.GetDefaultCellColouriser());
        }
    });
    connect(ui->colourTrichromatic, &QRadioButton::toggled, [&](bool checked)
    {
        if (checked) {
            ca.SetCellColouriser([](const double& value) -> unsigned
            {
                return value < -0.33 ? 0x00FF0000 : value > 0.33 ? 0x0000FF00 : 0x000000FF;
            });
        }
    });
    connect(ui->colourDumbChromatic, &QRadioButton::toggled, [&](bool checked)
    {
        if (checked) {
            ca.SetCellColouriser([](const double& value) -> unsigned
            {
                return (unsigned)value & 0x00FFFFFF;
            });
        }
    });
    connect(ui->colourBinaryGradient, &QRadioButton::toggled, [&](bool checked)
    {
        if (checked) {
            ca.SetCellColouriser([](const double& value) -> unsigned
            {
                return ((value + 1) / 2) * 0x00FFFFFF;
            });
        }
    });

    ui->colourMonochrome->setChecked(true);
}

void MainWindow::SetupRulesControlls()
{
    auto& ca = *ui->cellularAutomata;

    connect(ui->rulesConway, &QRadioButton::toggled, [&](bool checked)
    {
        if (checked) {
            ca.SetCellStepper(ca.GetDefaultCellStepper());
        }
    });
    connect(ui->rulesNeuralNet, &QRadioButton::toggled, [&](bool checked)
    {
        if (checked) {
            static NeuralNetwork network(3, 8, NeuralNetwork::InitialWeights::Random);
            static std::vector<double> neighbourhood;
            network = NeuralNetwork(3, 8, NeuralNetwork::InitialWeights::Random);
            ca.SetCellStepper([&](const CellularAutomata::GetNeighbourFunc& getCellValue) -> double
            {
                neighbourhood = {
                    getCellValue(-1, -1),
                    getCellValue(-1,  0),
                    getCellValue(-1,  1),
                    getCellValue( 0, -1),
                    getCellValue( 0,  1),
                    getCellValue( 1, -1),
                    getCellValue( 1,  0),
                    getCellValue( 1,  1),
                };
                network.ForwardPropogate(neighbourhood);
                return (neighbourhood[0] + neighbourhood[4] + getCellValue(0, 0)) / 3.0;
            });
        }
    });
    connect(ui->rulesMultipleNeighbourhoods, &QRadioButton::toggled, [&](bool checked)
    {
        if (checked) {
            static auto neighbourhood1 = Neighbourhood::CreateNeighbourhoodCoordinates(
            {
                { 0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0, },
                { 0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0, },
                { 0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0, },
                { 0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0, },
                { 0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0, },
                { 0,0,0,0,1,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,1,0,0,0,0, },
                { 0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0, },
                { 0,0,0,1,0,0,0,1,0,0,0,0,1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0, },
                { 0,0,1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0, },
                { 0,0,1,0,0,0,1,0,0,1,0,0,0,1,1,1,0,0,0,1,0,0,1,0,0,0,1,0,0, },
                { 0,1,0,0,0,1,0,0,1,0,0,1,1,0,0,0,1,1,0,0,1,0,0,1,0,0,0,1,0, },
                { 0,1,0,0,0,1,0,0,1,0,1,0,0,1,1,1,0,0,1,0,1,0,0,1,0,0,0,1,0, },
                { 0,1,0,0,0,1,0,1,0,0,1,0,1,0,0,0,1,0,1,0,0,1,0,1,0,0,0,1,0, },
                { 1,0,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,0,1,0,0,0,1, },
                { 1,0,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,0,1,0,0,0,1, },
                { 1,0,0,0,1,0,0,1,0,1,0,1,0,1,1,1,0,1,0,1,0,1,0,0,1,0,0,0,1, },
                { 0,1,0,0,0,1,0,1,0,0,1,0,1,0,0,0,1,0,1,0,0,1,0,1,0,0,0,1,0, },
                { 0,1,0,0,0,1,0,0,1,0,1,0,0,1,1,1,0,0,1,0,1,0,0,1,0,0,0,1,0, },
                { 0,1,0,0,0,1,0,0,1,0,0,1,1,0,0,0,1,1,0,0,1,0,0,1,0,0,0,1,0, },
                { 0,0,1,0,0,0,1,0,0,1,0,0,0,1,1,1,0,0,0,1,0,0,1,0,0,0,1,0,0, },
                { 0,0,1,0,0,0,1,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,0,0,0,1,0,0, },
                { 0,0,0,1,0,0,0,1,0,0,0,0,1,1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0, },
                { 0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,0,0,0, },
                { 0,0,0,0,1,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,1,0,0,0,0, },
                { 0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0, },
                { 0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0, },
                { 0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0, },
                { 0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0, },
                { 0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0, },
            });
            static auto neighbourhood2 = Neighbourhood::CreateNeighbourhoodCoordinates(
            {
                { 0,0,1,1,1,0,0, },
                { 0,1,0,0,0,1,0, },
                { 1,0,1,1,1,0,1, },
                { 1,0,1,0,1,0,1, },
                { 1,0,1,1,1,0,1, },
                { 0,1,0,0,0,1,0, },
                { 0,0,1,1,1,0,0, },
            });
            static auto neighbourhood3 = Neighbourhood::CreateNeighbourhoodCoordinates(
            {
                { 0,0,0,0,0,1,1,1,0,0,0,0,0 },
                { 0,0,0,1,1,1,1,1,1,1,0,0,0 },
                { 0,0,1,1,1,1,1,1,1,1,1,0,0 },
                { 0,1,1,1,1,0,0,0,1,1,1,1,0 },
                { 0,1,1,1,0,0,0,0,0,1,1,1,0 },
                { 1,1,1,0,0,0,0,0,0,0,1,1,1 },
                { 1,1,1,0,0,0,0,0,0,0,1,1,1 },
                { 1,1,1,0,0,0,0,0,0,0,1,1,1 },
                { 0,1,1,1,0,0,0,0,0,1,1,1,0 },
                { 0,1,1,1,1,0,0,0,1,1,1,1,0 },
                { 0,0,1,1,1,1,1,1,1,1,1,0,0 },
                { 0,0,0,1,1,1,1,1,1,1,0,0,0 },
                { 0,0,0,0,0,1,1,1,0,0,0,0,0 },
            });
            static auto neighbourhood4 = Neighbourhood::CreateNeighbourhoodCoordinates(
            {
                { 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0, },
                { 0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0, },
                { 0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0, },
                { 0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0, },
                { 0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0, },
                { 0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0, },
                { 0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0, },
                { 0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0, },
                { 0,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,1,1,0, },
                { 0,1,1,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,0, },
                { 0,1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,0, },
                { 1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,1,1,1, },
                { 1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,1,1, },
                { 1,1,1,1,0,0,0,1,0,0,1,0,0,1,1,1,0,0,1,0,0,1,0,0,0,1,1,1,1, },
                { 1,1,1,1,0,0,0,1,0,0,1,0,0,1,0,1,0,0,1,0,0,1,0,0,0,1,1,1,1, },
                { 1,1,1,1,0,0,0,1,0,0,1,0,0,1,1,1,0,0,1,0,0,1,0,0,0,1,1,1,1, },
                { 1,1,1,1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,1,1, },
                { 1,1,1,1,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,1,1,1, },
                { 0,1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,0,0,0,0,1,0,0,0,0,1,1,1,0, },
                { 0,1,1,1,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,1,1,1,0, },
                { 0,1,1,1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,1,1,0, },
                { 0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0, },
                { 0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0, },
                { 0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0, },
                { 0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0, },
                { 0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0, },
                { 0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0, },
                { 0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0, },
                { 0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0, },
            });
            ca.SetCellStepper([&](const CellularAutomata::GetNeighbourFunc& getCellValue) -> double
            {
                double neighbours1 = 0.0;
                for (auto coord : neighbourhood1) {
                    neighbours1 += getCellValue(coord.first, coord.second);
                }
                double neighbours2 = 0.0;
                for (auto coord : neighbourhood2) {
                    neighbours2 += getCellValue(coord.first, coord.second);
                }
                double neighbours3 = 0.0;
                for (auto coord : neighbourhood3) {
                    neighbours3 += getCellValue(coord.first, coord.second);
                }
                double neighbours4 = 0.0;
                for (auto coord : neighbourhood4) {
                    neighbours4 += getCellValue(coord.first, coord.second);
                }

                double value = getCellValue(0, 0);
                if (neighbours1 >= 0 && neighbours1 <= 17) {
                    value = 0;
                }
                if (neighbours1 >= 40 && neighbours1 <= 42) {
                    value = 1;
                }
                if (neighbours2 >= 10 && neighbours2 <= 13) {
                    value = 1;
                }
                if (neighbours3 >= 9 && neighbours3 <= 21) {
                    value = 0;
                }
                if (neighbours4 >= 78 && neighbours4 <= 89) {
                    value = 0;
                }
                if (neighbours4 > 108) {
                    value = 0;
                }
                return value;
            });
        }
    });
    ui->rulesConway->setChecked(true);
}

void MainWindow::SetupRandomiserControlls()
{
    ui->randMin->setValue(0.0);
    ui->randMin->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    ui->randMax->setValue(1.0);
    ui->randMax->setRange(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
    ui->randIntegersOnly->setChecked(true);

    connect(ui->randApply, &QPushButton::pressed, [&]()
    {
        if (ui->randIntegersOnly->isChecked()) {
            ui->cellularAutomata->Randomise<int>(ui->randMin->value(), ui->randMax->value());
        } else {
            ui->cellularAutomata->Randomise<double>(ui->randMin->value(), ui->randMax->value());
        }
    });
}

void MainWindow::SetupCellsControlls()
{
    ui->cellsWidthSpinner->setRange(0, 1000);
    ui->cellsWidthSpinner->setValue(100);
    ui->cellsHeightSpinner->setRange(0, 1000);
    ui->cellsHeightSpinner->setValue(100);

    connect(ui->cellsClear, &QPushButton::pressed, [&]() { ui->cellularAutomata->Clear(); });
    connect(ui->cellsSizeApplyButton, &QPushButton::pressed, [&]() { ui->cellularAutomata->SetDimensions(ui->cellsWidthSpinner->value(), ui->cellsHeightSpinner->value()); });
}

