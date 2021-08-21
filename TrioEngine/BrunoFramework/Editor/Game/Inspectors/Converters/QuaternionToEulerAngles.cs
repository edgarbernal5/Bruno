
using BrunoApi.Net.Maths;
using System;
using System.Globalization;
using System.Windows.Data;

namespace BrunoFramework.Editor.Game.Inspectors.Converters
{
    public class QuaternionToEulerAngles : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var rotation = (Quaternion)value;
            var eulerAngles = Quaternion.EulerAngles(rotation);
            return eulerAngles.ToDegrees();
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var eulerAngles = (Vector3)value;
            while(eulerAngles.X <= -360f)
            {
                eulerAngles.X += 360.0f;
            }
            while (eulerAngles.X >= 360f)
            {
                eulerAngles.X -= 360.0f;
            }

            while (eulerAngles.Y <= -360f)
            {
                eulerAngles.Y += 360.0f;
            }
            while (eulerAngles.Y >= 360f)
            {
                eulerAngles.Y -= 360.0f;
            }

            while (eulerAngles.Z <= -360f)
            {
                eulerAngles.Z += 360.0f;
            }
            while (eulerAngles.Z >= 360f)
            {
                eulerAngles.Z -= 360.0f;
            }
            eulerAngles = eulerAngles.ToRadians();
            var result = Quaternion.CreateFromAxisAngle(Vector3.UnitX, eulerAngles.X) *
                Quaternion.CreateFromAxisAngle(Vector3.UnitY, eulerAngles.Y) *
                Quaternion.CreateFromAxisAngle(Vector3.UnitZ, eulerAngles.Z);

            return result;
        }
    }
}
