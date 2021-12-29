
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

            var normalizedAngles = MathHelper.NormalizeAngles(eulerAngles);
            normalizedAngles = normalizedAngles.ToRadians();

            var result = Quaternion.CreateFromAxisAngle(Vector3.UnitX, normalizedAngles.X) *
                Quaternion.CreateFromAxisAngle(Vector3.UnitY, normalizedAngles.Y) *
                Quaternion.CreateFromAxisAngle(Vector3.UnitZ, normalizedAngles.Z);

            return result;
        }
    }
}
