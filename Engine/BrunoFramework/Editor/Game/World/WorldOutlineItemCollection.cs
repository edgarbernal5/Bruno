
using BrunoWindows;

namespace BrunoFramework.Editor.Game
{
    public class WorldOutlineItemCollection : ObservableChildCollection<WorldOutlineItem, WorldOutlineItem>
    {
        public new WorldOutlineItem Parent
        {
            get { return base.Parent; }
            protected internal set { base.Parent = value; }
        }

        public WorldOutlineItemCollection()
            : base(null)
        {
        }

        protected override WorldOutlineItem GetParent(WorldOutlineItem child)
        {
            return child.Parent;
        }

        protected override void SetParent(WorldOutlineItem child, WorldOutlineItem parent)
        {
            child.Parent = parent;
        }
    }
}
