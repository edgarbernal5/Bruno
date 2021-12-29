
using System;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace TrioWpfFramework.Controls
{
    /// <summary>
    /// Represents a control that displays an image from a multi-frame image source.
    /// </summary>
    /// <remarks>
    /// The <see cref="MultiFrameImage"/> can be used with image files which contain one picture
    /// in different resolutions (e.g. .ico files). <see cref="MultiFrameImage"/> renders the frame
    /// which is best for the current size of the <see cref="MultiFrameImage"/>.
    /// </remarks>
    public class MultiFrameImage : Icon
    {
        private BitmapFrame[] _frames;


        /// <summary>
        /// Initializes static members of the <see cref="MultiFrameImage"/> class.
        /// </summary>
        static MultiFrameImage()
        {
            SourceProperty.OverrideMetadata(typeof(MultiFrameImage), new FrameworkPropertyMetadata(OnSourceChanged));
        }

        private static void OnSourceChanged(DependencyObject dependencyObject, DependencyPropertyChangedEventArgs eventArgs)
        {
            var multiFrameImage = (MultiFrameImage)dependencyObject;
            multiFrameImage.UpdateFrameList();
        }

        private void UpdateFrameList()
        {
            var decoder = (Source as BitmapFrame)?.Decoder;
            if (decoder == null || decoder.Frames.Count == 0)
                return;

            // Order all frames by size, take the frame with the highest color depth per size.
            _frames = decoder.Frames
                             .GroupBy(f => f.PixelWidth * f.PixelHeight)
                             .OrderBy(g => g.Key)
                             .Select(g => g.OrderByDescending(f => f.Format.BitsPerPixel).First())
                             .ToArray();
        }


        /// <summary>
        /// Renders the optimal frame from the image.
        /// </summary>
        /// <param name="drawingContext">The drawing context used to render the control.</param>
        protected override void OnRender(DrawingContext drawingContext)
        {
            if (drawingContext == null)
                throw new ArgumentNullException(nameof(drawingContext));

            if (_frames == null || _frames.Length == 0)
            {
                base.OnRender(drawingContext);
            }
            else
            {
                double size = Math.Max(RenderSize.Width, RenderSize.Height);
                var frame = GetFrame(_frames, size);
                drawingContext.DrawImage(frame, new Rect(0, 0, RenderSize.Width, RenderSize.Height));
            }
        }


        private static BitmapFrame GetFrame(BitmapFrame[] frames, double size)
        {
            Debug.Assert(frames != null && frames.Length > 0);

            for (int i = 0; i < frames.Length; i++)
                if (frames[i].Width >= size && frames[i].Height >= size)
                    return frames[i];

            return frames[frames.Length - 1];
        }
    }
}