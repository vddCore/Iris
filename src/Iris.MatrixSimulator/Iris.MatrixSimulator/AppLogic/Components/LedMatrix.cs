namespace Iris.MatrixSimulator.AppLogic.Components;

using System.Numerics;
using Chroma.Graphics;

public class LedMatrix
{
    private RenderTarget _renderTarget;
    
    public Vector2 Position { get; set; }
    public Vector2 Size => new(_renderTarget.Width * Scale.X, _renderTarget.Height * Scale.Y);
    public Vector2 Scale { get; set; } = new(16, 16);

    public int Rows { get; }
    public int Columns { get; }
    public int PixelSpacing { get; }
    public Color[] Pixels { get; }

    public byte GlobalBrightness { get; set; } = 32;

    public LedMatrix(int rows, int columns, int pixelSpacing)
    {
        Rows = rows;
        Columns = columns;
        PixelSpacing = pixelSpacing;
        Pixels = new Color[columns * rows];

        _renderTarget = new RenderTarget(
            Rows + PixelSpacing * Rows - 1,
            Columns + PixelSpacing * Columns - 1
        ) { FilteringMode = TextureFilteringMode.NearestNeighbor };
    }

    public void Draw(RenderContext context)
    {
        context.RenderTo(_renderTarget, (c, t) =>
        {
            c.Clear(Color.Black);
            for (var y = 0; y < Rows; y++)
            {
                for (var x = 0; x < Columns; x++)
                {
                    c.Pixel(
                        x + PixelSpacing * x, 
                        y + PixelSpacing * y,
                        Pixels[y * Columns + x] with { A = GlobalBrightness }
                    );
                }
            }
        });
        
        context.DrawTexture(_renderTarget, Position, Scale);
    }

    public void SetPixel(int x, int y, Color color)
    {
        if (x < 0 || x >= Columns || y < 0 || y >= Rows)
            return;
        
        Pixels[y * Columns + x] = color;
    }
}