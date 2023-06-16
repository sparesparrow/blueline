// SpectrumVisualizer.h
#ifndef SPECTRUMVISUALIZER_H
#define SPECTRUMVISUALIZER_H

#include <QObject>
#include <QPainter>
#include <QQuickPaintedItem>
#include <QVariantList>

enum class SemitoneColor {
   ORANGE,
   VELVET,
   WINE,
   GRAY,
   GREEN,
   PURPLE,
   PINK,
   BLUE,
   LIGHT_BLUE,
   RED,
   BLACK,
   YELLOW,
   WHITE // Added missing enum value
};

class IVisualizer
{
public:
    virtual ~IVisualizer() = default;
};

class VisualizerQml : public IVisualizer , public QObject
{
   Q_OBJECT
public:
    VisualizerQml(QObject *parent = nullptr): QObject(parent)
    {}
    ~VisualizerQml() override = default;
signals:
    void colorChanged(QString color);
public slots:
    void updateVisualization(const QString& color)
    {
        if (color.isEmpty()) {
            // Error: color is invalid
            return;
        }
        float amplitude = color.toFloat();
        QColor ampColor = QColor::fromHslF(amplitude, 1.0, 0.5);
        // Emit the color changed signal
        emit colorChanged(color);
    }
};

class SpectrumVisualizer : public IVisualizer, public QQuickPaintedItem
{
   Q_OBJECT
public:
   static SpectrumVisualizer& getInstance(QQuickItem *parent = nullptr) {
       static SpectrumVisualizer instance(parent);
       return instance;
   }
   SpectrumVisualizer(const SpectrumVisualizer& other) = delete;  // Prevents copying
   void operator=(const SpectrumVisualizer&) = delete; // Prevents assignment
public slots:
   void paint(QPainter *painter) override {
       // Calculate the width of each rect
       int rectWidth = width() / m_numBars;

       // Calculate the maximum amplitude value
       qreal maxAmplitude = 0;
       if (!m_data.isEmpty()) { // Check if m_data is not empty before computing maxAmplitude
           for (int i = 0; i < m_data.size(); i++) {
               if (m_data[i] > maxAmplitude) {
                   maxAmplitude = m_data[i];
               }
           }
       }
       // Draw the rects
       for (int i = 0; i < m_numBars; i++) {
           // Calculate the height of each rect based on amplitude
           qint16 amplitude = m_data[i * m_data.size() / m_numBars];
           qreal rectHeight = amplitude / maxAmplitude * height();
           qreal opacity = amplitude / maxAmplitude;
           // Set the color and opacity of the painter based on amplitude
           painter->setOpacity(opacity);
           QColor color = QColor::fromHslF(amplitude / maxAmplitude, 1, 0.5);
           painter->setBrush(QBrush(color, Qt::SolidPattern));
           // Draw the rect
           painter->drawRect(QRect(i * rectWidth, height() - rectHeight, rectWidth, rectHeight));
       }
   }
   std::string colorForSemitone(SemitoneColor color) {
       switch (color) { // Fixed variable name from semitoneIndex to color
           case SemitoneColor::ORANGE: return "orange";          // C
           case SemitoneColor::VELVET: return "velvet";          // C#
           case SemitoneColor::WINE: return "wine";              // D
           case SemitoneColor::GRAY: return "gray";              // D#
           case SemitoneColor::GREEN: return "green";            // E
           case SemitoneColor::PURPLE: return "purple";          // F
           case SemitoneColor::PINK: return "pink";              // F#
           case SemitoneColor::BLUE: return "blue";              // G
           case SemitoneColor::LIGHT_BLUE: return "light blue";  // G#
           case SemitoneColor::RED: return "red";                // A
           case SemitoneColor::BLACK: return "black";            // A#
           case SemitoneColor::YELLOW: return "yellow";          // B
           default: return ""; // Added missing default case
       }
   }
   SemitoneColor semitoneToColor(double semitone) {
       return static_cast<SemitoneColor>(qRound(semitone) % 12);
   }
   // ...
private:
   SpectrumVisualizer(QQuickItem *parent = nullptr)
   : QQuickPaintedItem(parent), m_numBars(10) // Initialize m_numBars to some default value
   {}
   QVector<double> m_spectrumData;
   int m_numBars;
   QVector<qint16> m_data; // Added missing member variable
};

#endif // SPECTRUMVISUALIZER_H
