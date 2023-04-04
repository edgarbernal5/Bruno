
using BrunoApi.Net.Graphics;
using BrunoApi.Net.Graphics.Core;
using BrunoApi.Net.Maths;
using System;

namespace BrunoFramework.Graphics.Editor
{
    public class DebugRenderer
    {
        public Effect Effect
        {
            get => m_effect;
            set {
                m_effect = value;
            }
        }
        private Effect m_effect;
        
        private LineBatcher m_lineBatch;

        public DebugRenderer()
        {
            m_lineBatch = new LineBatcher(m_effect);
        }

        public void Clear()
        {
            m_lineBatch.Clear();
        }

        public void Render(RenderContext renderContext)
        {
            var worldViewProj = renderContext.Camera.ViewProjection;
            m_effect.Parameters["gWorldViewProj"].SetValue(worldViewProj);
            m_effect.Techniques[0].Passes[0].Apply();

            m_lineBatch.Render(renderContext);
        }

        public void AddLine(Vector3 startPoint, Vector3 endPoint, ColorRGBA8 color)
        {
            m_lineBatch.Add(startPoint, endPoint, color);
        }

        public void AddBoundingBox(BoundingBox box, ColorRGBA8 color)
        {
            const float LineWidth = 0.25f;

            var min = box.Center - box.Extents;
            var max = box.Center + box.Extents;

            m_lineBatch.Add(min, min + Vector3.Right * LineWidth, color);
            m_lineBatch.Add(min, min + Vector3.Backward * LineWidth, color);
            m_lineBatch.Add(min, min + Vector3.Up * LineWidth, color);

            var p = new Vector3(min.X, max.Y, min.Z);
            m_lineBatch.Add(p, p + Vector3.Right * LineWidth, color);
            m_lineBatch.Add(p, p + Vector3.Backward * LineWidth, color);
            m_lineBatch.Add(p, p + Vector3.Down * LineWidth, color);

            p = new Vector3(max.X, min.Y, min.Z);
            m_lineBatch.Add(p, p + Vector3.Left * LineWidth, color);
            m_lineBatch.Add(p, p + Vector3.Backward * LineWidth, color);
            m_lineBatch.Add(p, p + Vector3.Up * LineWidth, color);

            p = new Vector3(max.X, max.Y, min.Z);
            m_lineBatch.Add(p, p + Vector3.Left * LineWidth, color);
            m_lineBatch.Add(p, p + Vector3.Backward * LineWidth, color);
            m_lineBatch.Add(p, p + Vector3.Down * LineWidth, color);

            //
            m_lineBatch.Add(max, max + Vector3.Left * LineWidth, color);
            m_lineBatch.Add(max, max + Vector3.Forward * LineWidth, color);
            m_lineBatch.Add(max, max + Vector3.Down * LineWidth, color);

            p = new Vector3(min.X, min.Y, max.Z);
            m_lineBatch.Add(p, p + Vector3.Right * LineWidth, color);
            m_lineBatch.Add(p, p + Vector3.Forward * LineWidth, color);
            m_lineBatch.Add(p, p + Vector3.Up * LineWidth, color);

            p = new Vector3(min.X, max.Y, max.Z);
            m_lineBatch.Add(p, p + Vector3.Right * LineWidth, color);
            m_lineBatch.Add(p, p + Vector3.Forward * LineWidth, color);
            m_lineBatch.Add(p, p + Vector3.Down * LineWidth, color);

            p = new Vector3(max.X, min.Y, max.Z);
            m_lineBatch.Add(p, p + Vector3.Left * LineWidth, color);
            m_lineBatch.Add(p, p + Vector3.Forward * LineWidth, color);
            m_lineBatch.Add(p, p + Vector3.Up * LineWidth, color);
        }
    }
}
