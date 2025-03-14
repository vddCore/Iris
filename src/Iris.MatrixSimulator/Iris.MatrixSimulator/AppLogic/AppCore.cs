namespace Iris.MatrixSimulator.AppLogic;

using System.Numerics;
using Chroma;
using Chroma.ContentManagement;
using Chroma.Graphics;
using Chroma.Graphics.Accelerated;
using Iris.MatrixSimulator.AppLogic.Components;

public class AppCore : Game
{
    private Effect _crtEffect = null!;
    private RenderTarget _renderTarget = null!;
    
    private LedMatrix _ledMatrix = null!;
    private double _t;

    public AppCore()
        : base(new(false, false))
    {
    }
    
    protected override void Initialize(IContentProvider content)
    {
        _crtEffect = content.Load<Effect>("shaders/crt.glsl")!;
        
        _ledMatrix = new LedMatrix(8, 8, 0);
        _ledMatrix.Scale = new(64, 64);
        
        Window.Mode.SetWindowed(
            (ushort)(_ledMatrix.Size.X + _ledMatrix.Scale.X * _ledMatrix.PixelSpacing * 2),
            (ushort)(_ledMatrix.Size.Y + _ledMatrix.Scale.Y * _ledMatrix.PixelSpacing * 2),
            true
        );

        _renderTarget = new RenderTarget(Window.Size);
        
        _ledMatrix.Position = new(
            Window.Width / 2 - _ledMatrix.Size.X / 2,
            Window.Height / 2 - _ledMatrix.Size.Y / 2
        );

        for (var y = 0; y < 8; y++)
        {
            for (var x = 0; x < 8; x++)
            {
                _ledMatrix.SetPixel(x, y, Color.White);
            }
        }
    }

    protected override void Draw(RenderContext context)
    {
        context.RenderTo(_renderTarget, (c, _) =>
        {
            _ledMatrix.Draw(c);
        });
        
        _crtEffect.Activate();
        {
            context.DrawTexture(_renderTarget, Vector2.Zero);
        }
        Shader.Deactivate();
    }

    protected override void Update(float delta)
    {
        for (var y = 0; y < 8; y++)
        {
            for (var x = 0; x < 8; x++)
            {
                var r = (byte)(255.0 * (Math.Cos(Math.Sin(x / 2.0 + 1.0 + _t / 2.0) + Math.Sin(y / 2.0 + 4.0 + _t / 1.25) + _t / 0.85))) * 0;
                var g = (byte)(255.0 * (Math.Sin(Math.Cos(x / 2.0 + 2.0 + _t / 2.0) + Math.Sin(y / 2.0 + 2.0 + _t / 1.50) + _t / 0.95)));
                var b = (byte)(127.0 * (1.0 - Math.Sin(Math.Sin(y / 2.0 + 4.0 + _t / 2.0) + Math.Cos(x / 2.0 + 1.0 + _t / 2.00) + _t / 1.05)));
                
                _ledMatrix.SetPixel(x, y, new Color((byte)r, g, b));
            }
        }
        
        _t += delta * 2;
    }
}